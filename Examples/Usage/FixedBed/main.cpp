//==============================================================================
// Name        : main.cpp
// Authors     : Joao Flavio Vasconcellos
// Version     : 2.0.0
// Description : Same equilibrium data, different fixed-bed predictions.
//
// Copyright   : Copyright (C) 2026 Joao Flavio Vasconcellos
// Licence     : MIT License (see LICENSE.txt)
// SPDX-License-Identifier: MIT
//==============================================================================

// Illustrative example of the SoftwareX paper (Section 3, Fig. 2).
//
// 1. Four isotherms (Langmuir, Sips, Toth, Redlich-Peterson) are fitted to the
//    same synthetic equilibrium data set, generated from a Toth isotherm with
//    2 % multiplicative noise. Fitting uses a Nelder-Mead search on the
//    logarithms of the parameters and the library's with_parameters().
// 2. Each fitted model is used, without any other change, in the
//    equilibrium-dispersive model of a fixed bed,
//       d/dt [eps C + rho_b q(C)] + u dC/dz = eps D d2C/dz2,
//    discretised by finite volumes (upwind convection, central dispersion)
//    and integrated with implicit Euler. Each time step is solved by Newton's
//    method, whose Jacobian uses the exact derivative dq/dC of the library.
// 3. Two operations are simulated: loading followed by elution, and the
//    injection of a dilute pulse.
//
// Usage: example_Usage_FixedBed [output-directory]
// Output: fig2_data.csv, fig2_isotherms.csv, fig2_slopes.csv,
//         fig2_loading.csv, fig2_pulse.csv (plotted by plot.py)

//==============================================================================
// C++ standard library headers
//==============================================================================

#include <algorithm>
#include <array>
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iomanip>
#include <iostream>
#include <limits>
#include <string>
#include <tuple>
#include <vector>

//==============================================================================
// Isotherm++ headers
//==============================================================================

#include <isotherm/isotherm.hpp>

using namespace isotherm;

namespace {

//------------------------------------------------------------------------------
// Synthetic equilibrium data: C in mg/L, q in mg/g.
//------------------------------------------------------------------------------
constexpr std::array<Real, 10> data_c{2, 5, 10, 20, 35, 50, 75, 100, 140, 200};
constexpr std::array<Real, 10> data_q{5.51, 11.02, 17.10, 24.75, 32.27,
                                      36.70, 43.00, 47.98, 50.43, 54.38};

//------------------------------------------------------------------------------
// Fixed bed (SI units except C in mg/L, q in mg/g, rho_b in g/L).
//------------------------------------------------------------------------------
struct Bed {
    Real length = 0.1;          // m
    Real eps = 0.4;             // bed porosity
    Real rho_b = 500.0;         // g/L
    Real u = 1.0e-4;            // superficial velocity, m/s
    Real peclet = 500.0;        // u L / D
    std::size_t cells = 150;
    Real dt = 300.0;            // s
    [[nodiscard]] Real dz() const { return length / static_cast<Real>(cells); }
    [[nodiscard]] Real dispersion() const { return u * length / peclet; }
};

constexpr Real tiny = 1.0e-300;

//------------------------------------------------------------------------------
// Nelder-Mead on the logarithms of the parameters (keeps them positive).
//------------------------------------------------------------------------------
template<class Model>
Real sum_of_squares(const Model& model) {
    Real s = 0.0;
    for (std::size_t i = 0; i < data_c.size(); ++i) {
        const Real r = data_q[i] - model.evaluate(data_c[i]);
        s += r * r;
    }
    return s;
}

template<class Model>
Model fit(const Model& start) {
    constexpr std::size_t n = model_traits<Model>::parameter_count;
    using Point = std::array<Real, n>;
    const auto objective = [&](const Point& x) {
        Point p{};
        for (std::size_t i = 0; i < n; ++i) p[i] = std::exp(x[i]);
        try {
            return sum_of_squares(with_parameters(start, p));
        } catch (const isotherm_error&) {
            return std::numeric_limits<Real>::max();
        }
    };
    std::array<Point, n + 1> simplex{};
    std::array<Real, n + 1> value{};
    const auto p0 = parameters(start);
    for (std::size_t k = 0; k <= n; ++k) {
        for (std::size_t i = 0; i < n; ++i) simplex[k][i] = std::log(p0[i]);
        if (k > 0) simplex[k][k - 1] += 0.3;
        value[k] = objective(simplex[k]);
    }
    for (int iteration = 0; iteration < 4000; ++iteration) {
        std::array<std::size_t, n + 1> order{};
        for (std::size_t k = 0; k <= n; ++k) order[k] = k;
        std::sort(order.begin(), order.end(),
                  [&](std::size_t a, std::size_t b) { return value[a] < value[b]; });
        const std::size_t best = order[0], worst = order[n], second = order[n - 1];
        if (value[worst] - value[best] < 1e-14 * (1.0 + value[best])) break;
        Point centroid{};
        for (std::size_t k = 0; k <= n; ++k)
            if (k != worst)
                for (std::size_t i = 0; i < n; ++i) centroid[i] += simplex[k][i] / n;
        const auto towards = [&](Real factor) {
            Point x{};
            for (std::size_t i = 0; i < n; ++i)
                x[i] = centroid[i] + factor * (simplex[worst][i] - centroid[i]);
            return x;
        };
        const Point reflected = towards(-1.0);
        const Real fr = objective(reflected);
        if (fr < value[best]) {
            const Point expanded = towards(-2.0);
            const Real fe = objective(expanded);
            if (fe < fr) { simplex[worst] = expanded; value[worst] = fe; }
            else { simplex[worst] = reflected; value[worst] = fr; }
        } else if (fr < value[second]) {
            simplex[worst] = reflected; value[worst] = fr;
        } else {
            const Point contracted = towards(0.5);
            const Real fc = objective(contracted);
            if (fc < value[worst]) { simplex[worst] = contracted; value[worst] = fc; }
            else {
                for (std::size_t k = 0; k <= n; ++k) {
                    if (k == best) continue;
                    for (std::size_t i = 0; i < n; ++i)
                        simplex[k][i] = simplex[best][i] + 0.5 * (simplex[k][i] - simplex[best][i]);
                    value[k] = objective(simplex[k]);
                }
            }
        }
    }
    const std::size_t best = static_cast<std::size_t>(
        std::min_element(value.begin(), value.end()) - value.begin());
    Point p{};
    for (std::size_t i = 0; i < n; ++i) p[i] = std::exp(simplex[best][i]);
    return with_parameters(start, p);
}

template<class Model>
Real r_squared(const Model& model) {
    Real mean = 0.0;
    for (Real q : data_q) mean += q / data_q.size();
    Real total = 0.0;
    for (Real q : data_q) total += (q - mean) * (q - mean);
    return 1.0 - sum_of_squares(model) / total;
}

//------------------------------------------------------------------------------
// Implicit finite-volume solver of the fixed bed.
// Returns the outlet concentration at every time step.
//------------------------------------------------------------------------------
template<class Model>
std::vector<Real> simulate(const Model& model, const Bed& bed, Real t_end,
                           const std::function<Real(Real)>& inlet) {
    const std::size_t n = bed.cells;
    const Real dz = bed.dz();
    const Real a = bed.u / dz;                                 // convection
    const Real d = bed.eps * bed.dispersion() / (dz * dz);     // dispersion
    std::vector<Real> c(n, 0.0), accumulated(n, 0.0);
    std::vector<Real> residual(n), lower(n), diag(n), upper(n), delta(n);
    const auto steps = static_cast<std::size_t>(std::llround(t_end / bed.dt));
    std::vector<Real> outlet;
    outlet.reserve(steps + 1);
    outlet.push_back(0.0);
    for (std::size_t step = 1; step <= steps; ++step) {
        const Real c_in = inlet(static_cast<Real>(step) * bed.dt);
        for (std::size_t i = 0; i < n; ++i)
            accumulated[i] = bed.eps * c[i] + bed.rho_b * model.evaluate(c[i]);
        for (int newton = 0; newton < 50; ++newton) {
            Real largest = 0.0;
            for (std::size_t i = 0; i < n; ++i) {
                const Real west = (i == 0) ? c_in : c[i - 1];
                const Real east = (i + 1 == n) ? c[i] : c[i + 1];
                residual[i] = (bed.eps * c[i] + bed.rho_b * model.evaluate(c[i]) - accumulated[i]) / bed.dt
                            + a * (c[i] - west) - d * (east - 2.0 * c[i] + west);
                const Real slope = model.derivative(std::max(c[i], tiny));
                diag[i] = (bed.eps + bed.rho_b * slope) / bed.dt + a + 2.0 * d - ((i + 1 == n) ? d : 0.0);
                lower[i] = (i == 0) ? 0.0 : -a - d;
                upper[i] = (i + 1 == n) ? 0.0 : -d;
            }
            // Thomas algorithm for J delta = -residual.
            for (std::size_t i = 1; i < n; ++i) {
                const Real m = lower[i] / diag[i - 1];
                diag[i] -= m * upper[i - 1];
                residual[i] -= m * residual[i - 1];
            }
            delta[n - 1] = -residual[n - 1] / diag[n - 1];
            for (std::size_t i = n - 1; i-- > 0;)
                delta[i] = (-residual[i] - upper[i] * delta[i + 1]) / diag[i];
            for (std::size_t i = 0; i < n; ++i) {
                const Real updated = std::max(c[i] + delta[i], 0.0);
                largest = std::max(largest, std::abs(updated - c[i]));
                c[i] = updated;
            }
            if (largest < 1e-12) break;
        }
        outlet.push_back(c[n - 1]);
    }
    return outlet;
}

} // namespace

int main(int argc, char* argv[]) {
    const std::filesystem::path out = (argc > 1) ? argv[1] : ".";
    std::filesystem::create_directories(out);

    // 1. Fit the four isotherms to the same data.
    const auto langmuir = fit(models::Langmuir{60.0, 0.05});
    const auto sips = fit(models::Sips{70.0, 0.03, 1.2});
    const auto toth = fit(models::Toth{78.0, 0.15, 0.6});
    const auto redlich_peterson = fit(models::RedlichPeterson{3.0, 0.1, 0.85});
    const auto models = std::make_tuple(langmuir, sips, toth, redlich_peterson);
    const std::array<std::string, 4> names{"Langmuir", "Sips", "Toth", "Redlich-Peterson"};

    std::cout << std::setprecision(6);
    std::size_t index = 0;
    std::apply([&](const auto&... m) {
        ((std::cout << std::left << std::setw(18) << names[index++] << " parameters:",
          [&] { for (Real p : parameters(m)) std::cout << ' ' << p; }(),
          std::cout << "   R2 = " << r_squared(m) << '\n'), ...);
    }, models);

    // 2. Equilibrium data, fitted isotherms and slopes.
    {
        std::ofstream f(out / "fig2_data.csv");
        f << "concentration,uptake\n" << std::setprecision(17);
        for (std::size_t i = 0; i < data_c.size(); ++i) f << data_c[i] << ',' << data_q[i] << '\n';
    }
    {
        std::ofstream f(out / "fig2_isotherms.csv");
        f << "concentration,Langmuir,Sips,Toth,Redlich-Peterson\n" << std::setprecision(17);
        for (int k = 0; k <= 400; ++k) {
            const Real c = 220.0 * k / 400.0;
            f << c;
            std::apply([&](const auto&... m) { ((f << ',' << m.evaluate(c)), ...); }, models);
            f << '\n';
        }
    }
    {
        std::ofstream f(out / "fig2_slopes.csv");
        f << "concentration,Langmuir,Sips,Toth,Redlich-Peterson\n" << std::setprecision(17);
        for (int k = 0; k <= 400; ++k) {
            const Real c = std::pow(10.0, -2.0 + 4.4 * k / 400.0);
            f << c;
            std::apply([&](const auto&... m) { ((f << ',' << m.derivative(c)), ...); }, models);
            f << '\n';
        }
    }

    // 3. Fixed bed: loading (C0 = 100 mg/L) until 100 h, then elution.
    const Bed bed;
    constexpr Real hour = 3600.0;
    constexpr Real c0 = 100.0;
    const auto loading = [&](Real t) { return t <= 100.0 * hour ? c0 : 0.0; };
    std::vector<std::vector<Real>> load_runs;
    std::apply([&](const auto&... m) { (load_runs.push_back(simulate(m, bed, 400.0 * hour, loading)), ...); }, models);

    // 4. Pulse: 5 mg/L during 20 h.
    constexpr Real c_pulse = 5.0;
    const auto pulse = [&](Real t) { return t <= 20.0 * hour ? c_pulse : 0.0; };
    std::vector<std::vector<Real>> pulse_runs;
    std::apply([&](const auto&... m) { (pulse_runs.push_back(simulate(m, bed, 1200.0 * hour, pulse)), ...); }, models);

    const auto write_runs = [&](const std::string& file, const std::vector<std::vector<Real>>& runs, Real scale) {
        std::ofstream f(out / file);
        f << "time_h,Langmuir,Sips,Toth,Redlich-Peterson\n" << std::setprecision(10);
        for (std::size_t k = 0; k < runs[0].size(); ++k) {
            f << static_cast<Real>(k) * bed.dt / hour;
            for (const auto& run : runs) f << ',' << run[k] / scale;
            f << '\n';
        }
    };
    write_runs("fig2_loading.csv", load_runs, c0);
    write_runs("fig2_pulse.csv", pulse_runs, c_pulse);

    std::cout << "\n" << std::left << std::setw(18) << "Model"
              << "  breakthrough C/C0=0.05 (h)   pulse peak (h)   peak C/Cinj\n";
    for (std::size_t k = 0; k < names.size(); ++k) {
        Real t5 = std::numeric_limits<Real>::quiet_NaN();
        for (std::size_t s = 0; s < load_runs[k].size(); ++s)
            if (load_runs[k][s] / c0 > 0.05) { t5 = static_cast<Real>(s) * bed.dt / hour; break; }
        const auto peak = std::max_element(pulse_runs[k].begin(), pulse_runs[k].end());
        const Real t_peak = static_cast<Real>(peak - pulse_runs[k].begin()) * bed.dt / hour;
        std::cout << std::left << std::setw(18) << names[k] << "  " << std::setw(28) << t5
                  << std::setw(17) << t_peak << *peak / c_pulse << '\n';
    }
    std::cout << "\nCSV files written to " << std::filesystem::absolute(out) << '\n';
    return 0;
}
