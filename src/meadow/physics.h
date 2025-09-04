#pragma once

#include <utility>

enum class NewtonDynamicsIntegrator {
    euler,
    semi_implicit_euler,
    velocity_verlet, // Assumes that acceleration is not dependent on velocity.
    runge_kutta_2,
    runge_kutta_4
};

// Return next position and velocity.
template<class T, class Duration, class AccelerationFn>
std::pair<T, T> integrate_newton_dynamics(
  NewtonDynamicsIntegrator ndi, const T& pos, const T& v, const Duration dt, AccelerationFn&& acceleration_fn
)
{
    switch (ndi) {
    case NewtonDynamicsIntegrator::euler: {
        const auto a = acceleration_fn(pos, v);
        const T next_v = v + a * dt;
        const T next_pos = pos + v * dt;
        return std::pair(next_pos, next_v);
    } break;
    case NewtonDynamicsIntegrator::semi_implicit_euler: {
        const auto a = acceleration_fn(pos, v);
        const T next_v = v + a * dt;
        const T next_pos = pos + next_v * dt;
        return std::pair(next_pos, next_v);
    } break;
    case NewtonDynamicsIntegrator::velocity_verlet: {
        const T a = acceleration_fn(pos, v);
        const T next_pos = pos + v * dt + a * dt * dt / 2;
        const T next_a = acceleration_fn(next_pos, v);
        const T next_v = v + (a + next_a) / 2 * dt;
        return std::pair(next_pos, next_v);
    } break;
    case NewtonDynamicsIntegrator::runge_kutta_2: {
        const auto k1x = v;
        const auto k1v = acceleration_fn(pos, v);
        const auto k2x = v + k1v * dt / 2;
        const auto k2v = acceleration_fn(pos + k1x * dt / 2, v + k1v * dt / 2);
        const auto next_pos = pos + k2x * dt;
        const auto next_v = v + k2v * dt;
        return std::pair(next_pos, next_v);
    } break;
    case NewtonDynamicsIntegrator::runge_kutta_4: {
        const auto k1x = v;
        const auto k1v = acceleration_fn(pos, v);
        const auto k2x = v + k1v * dt / 2;
        const auto k2v = acceleration_fn(pos + k1x * dt / 2, v + k1v * dt / 2);
        const auto k3x = v + k2v * dt / 2;
        const auto k3v = acceleration_fn(pos + k2x * dt / 2, v + k2v * dt / 2);
        const auto k4x = v + k3v * dt;
        const auto k4v = acceleration_fn(pos + k3x * dt, v + k3v * dt);
        const auto next_pos = pos + (k1x + 2 * k2x + 2 * k3x + k4x) * dt / 6;
        const auto next_v = v + (k1v + 2 * k2v + 2 * k3v + k4v) * dt / 6;
        return std::pair(next_pos, next_v);
    } break;
    }
}
