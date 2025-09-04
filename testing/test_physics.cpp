#include "meadow/physics.h"

#include "meadow/cppext.h"
#include "meadow/math.h"
#include "meadow/matlab.h"

#include <gtest/gtest.h>
#include <magic_enum/magic_enum.hpp>

// These tests don't test anything, they were used to manually examining the results.

static void test_physics_integrators_spring_mass(const double m, const double c, const double k)
{
    const auto calculate_acceleration = [&](double x, double v) {
        return (-c * v - k * x) / m;
    };

    // Initial conditions.
    constexpr double x0 = 2;
    constexpr double v0 = 1;

    // Derived quantities.
    const double omega_0 = sqrt(k / m);
    const double gamma = c / (2 * m);
    println("omega_0/2π = {} Hz (natural frequency)", omega_0 / (2 * num::pi));
    println("gamma = {} (damping)", gamma);

    enum class Damping {
        underdamped,
        critically_damped,
        overdamped
    };

    Damping damping;
    double C1 = NAN, C2 = NAN, r1 = NAN, r2 = NAN;
    double omega_d = NAN;

    if (gamma < omega_0) {
        damping = Damping::underdamped;
        omega_d = sqrt(square(omega_0) - square(gamma));
        println("omega_d/2π = {} Hz (actual frequency)", omega_d / (2 * num::pi));
    } else if (gamma > omega_0) {
        damping = Damping::overdamped;
        const double D = sqrt(square(gamma) - square(omega_0));
        r1 = -gamma - D;
        r2 = -gamma + D;
        C1 = (v0 - r2 * x0) / (r1 - r2);
        C2 = (r1 * x0 - v0) / (r1 - r2);
    } else {
        damping = Damping::critically_damped;
    }
    println("Damping: {}", magic_enum::enum_name(damping));

    // Simulation setup.
    constexpr size_t points_per_period = 24;
    constexpr size_t N = 7 * points_per_period;
    const double f_d = omega_0 / (2 * num::pi);
    const double T_d = 1 / f_d;
    const double dt = T_d / points_per_period;

    // Simulate.
    vector<double> ts;
    ts.reserve(N);
    constexpr size_t NI = magic_enum::enum_values<NewtonDynamicsIntegrator>().size();
    vector<vector<double>> xs(NI + 1), vs(NI + 1);
    struct State {
        NewtonDynamicsIntegrator integrator;
        double x = x0;
        double v = v0;
        double error_x = 0;
        double error_v = 0;

        explicit State(NewtonDynamicsIntegrator integrator_arg)
            : integrator(integrator_arg)
        {
        }
    };
    for (size_t i : vi::iota(0u, NI + 1)) {
        xs[i].reserve(N);
        vs[i].reserve(N);
    }
    vector<State> states;
    for (auto ndi : magic_enum::enum_values<NewtonDynamicsIntegrator>()) {
        states.emplace_back(ndi);
    }

    for (size_t n : vi::iota(0u, N)) {
        const double t = n * dt;
        // Analytics solution.
        double x_t = NAN;
        double v_t = NAN;
        switch (damping) {
        case Damping::underdamped:
            x_t = exp(-gamma * t) * (x0 * cos(omega_d * t) + (v0 + gamma * x0) / omega_d * sin(omega_d * t));
            v_t = exp(-gamma * t)
                * (v0 * cos(omega_d * t) - (square(omega_0) * x0 + gamma * v0) / omega_d * sin(omega_d * t));
            break;
        case Damping::critically_damped:
            x_t = exp(-gamma * t) * (x0 + (v0 + gamma * x0) * t);
            v_t = exp(-gamma * t) * (v0 - (v0 + gamma * x0) * gamma * t);
            break;
        case Damping::overdamped:
            x_t = C1 * exp(r1 * t) + C2 * exp(r2 * t);
            v_t = C1 * r1 * exp(r1 * t) + C2 * r2 * exp(r2 * t);
            break;
        }
        ts.push_back(t);
        xs[0].push_back(x_t);
        vs[0].push_back(v_t);
        for (size_t i : vi::iota(0u, NI)) {
            auto& state = states[i];
            xs[i + 1].push_back(state.x);
            vs[i + 1].push_back(state.v);
            state.error_x += square(state.x - x_t);
            state.error_v += square(state.v - v_t);
            std::tie(state.x, state.v) =
              integrate_newton_dynamics(state.integrator, state.x, state.v, dt, calculate_acceleration);
        }
    }
#if 0
    EXPECT_TRUE(matlab::saveAscii("/tmp/ts", span(ts)));
    EXPECT_TRUE(matlab::saveAscii("/tmp/xs", MatrixReader<double>(N, NI + 1, [&](size_t r_, size_t c_) {
                                      return xs[c_][r_];
                                  })));
    EXPECT_TRUE(matlab::saveAscii("/tmp/vs", MatrixReader<double>(N, NI + 1, [&](size_t r_, size_t c_) {
                                      return vs[c_][r_];
                                  })));
#endif
    for (auto& state : states) {
        state.error_x = sqrt(state.error_x / N);
        state.error_v = sqrt(state.error_v / N);
    }
    ra::sort(states, {}, &State::error_x);
    println("Integrators by position errors:");
    for (size_t i : vi::iota(0u, NI)) {
        println("{}: {:.2f} dB {}", i, matlab::mag2db(states[i].error_x), magic_enum::enum_name(states[i].integrator));
    }
    ra::sort(states, {}, &State::error_v);
    println("Integrators by velocity errors:");
    for (size_t i : vi::iota(0u, NI)) {
        println("{}: {:.2f} dB {}", i, matlab::mag2db(states[i].error_v), magic_enum::enum_name(states[i].integrator));
    }
}

TEST(physics, integrators_spring_mass_no_damping)
{
    test_physics_integrators_spring_mass(1, 0, 4);
}

TEST(physics, integrators_spring_mass_underdamped)
{
    test_physics_integrators_spring_mass(1, 1.5, 4);
}

TEST(physics, integrators_spring_mass_critically_damped)
{
    test_physics_integrators_spring_mass(1, 4, 4);
}

TEST(physics, integrators_spring_mass_overdamped)
{
    test_physics_integrators_spring_mass(1, 8, 4);
}
