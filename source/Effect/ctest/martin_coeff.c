//
// Created by maple on 25-5-8.
//
// #include <stdio.h>
#include "martin_coeff.h"
#include "math.h"
#include "helpers.h"
#include "analog_func.h"


filter_coeff get1LowPass(const double w0)
{
    const double fc = w0 / pi;
    const double fm = 0.5 * sqrt(fc * fc + 1);
    const double phim = 1 - cos(pi * fm);

    const double a1 = -exp(-w0);

    const double alpha = -2 * a1 / pow(1 + a1, 2);
    const double k = (fc * fc) / (fc * fc + fm * fm);
    const double beta = k * alpha + (k - 1) / phim;
    const double b_temp = -beta / (1 + beta + sqrt(1 + 2 * beta));

    const double b0 = (1.0 + a1) / (1.0 + b_temp);
    const double b1 = b_temp * b0;
    // return {1.0, a1, b0, b1};

    filter_coeff filtercoeff ={1.0, a1, b0,b1};

    return filtercoeff;
}

filter_coeff get1HighPass(const double w0)
{
    const double wm = w0 * 0.5;
    double phim[2] = {1 - pow(sin(wm / 2), 2), pow(sin(wm / 2), 2)};

    const double a1 = -exp(-w0);

    double A[2] = {pow(1.0 + a1, 2), pow(1.0 - a1, 2)};
    const double B1 = (wm * wm) / (wm * wm + w0 * w0) * (A[0] * phim[0] + A[1] * phim[1]) / phim[1];
    const double b0 = 0.5 * sqrt(B1);

    filter_coeff filtercoeff;
    filtercoeff.coeff[0]=1.0;
    filtercoeff.coeff[1]=a1;
    filtercoeff.coeff[2]=b0;
    filtercoeff.coeff[3]=-b0;
    return filtercoeff;
}

filter_coeff get1TiltShelf(const double w0, const double g)
{
    const double fc = w0 / pi;
    const double fm = fc * 0.75;
    const double phim = 1 - cos(pi * fm);
    const double alpha = 2 / pi2 * (1 / pow(fm, 2) + 1 / g / pow(fc, 2)) - 1 / phim;
    const double beta = 2 / pi2 * (1 / pow(fm, 2) + g / pow(fc, 2)) - 1 / phim;

    const double a1 = -alpha / (1 + alpha + sqrt(1 + 2 * alpha));

    const double b_temp = -beta / (1 + beta + sqrt(1 + 2 * beta));

    const double b0 = (1 + a1) / (1 + b_temp) / sqrt(g);
    const double b1 = b_temp * b0;
    // return {1.0, a1, b0, b1};

    filter_coeff filtercoeff;
    filtercoeff.coeff[0]=1.0;
    filtercoeff.coeff[1]=a1;
    filtercoeff.coeff[2]=b0;
    filtercoeff.coeff[3]=b1;
    return filtercoeff;
}

filter_coeff get1LowShelf(const double w0, const double g)
{
    filter_coeff ab = get1TiltShelf(w0, 1.0 / g);
    const double A = sqrt(g);
    // return {1.0, ab[1], ab[2] * A, ab[3] * A};

    filter_coeff filtercoeff = {{1.0, ab.coeff[1], ab.coeff[2] * A, ab.coeff[3] * A}};
    return filtercoeff;
}

filter_coeff get1HighShelf(const double w0, const double g)
{
    filter_coeff ab = get1TiltShelf(w0, g);
    const double A = sqrt(g);
    // return {1.0, ab[1], ab[2] * A, ab[3] * A};

    filter_coeff filtercoeff = {{1.0, ab.coeff[1], ab.coeff[2] * A, ab.coeff[3] * A}};
    return filtercoeff;
}

filter_coeff get2LowPass(const double w0, const double q)
{
    filter_middle_coeff a = solve_a(w0, 0.5 / q, 1);
    filter_middle_coeff A = get_AB(a);
    filter_coeff ws={0};
    if (w0 > pi / 32) {
        ws.coeff[0] = 0;
        ws.coeff[1] = 0.5 * w0;
        ws.coeff[2] = w0;
    } else {
        // ws = {pi, w0, 0.5 * (pi + w0)};
        ws.coeff[0] = pi;
        ws.coeff[1] = w0;
        ws.coeff[2] = 0.5 * (pi + w0);
    }

    filter_middle_coeff phi[3]={0};
    filter_middle_coeff res={0};
    for (size_t i = 0; i < 3; ++i) {
        phi[i] = get_phi(ws.coeff[i]);
        res.coeff[i] = get2LowPassMagnitude2(w0, q, ws.coeff[i]) * dot_product(phi[i], A);
    }
    filter_middle_coeff B = linear_solve(phi, res);
    filter_middle_coeff b = get_ab(B);
    // return {a[0], a[1], a[2], b[0], b[1], b[2]};

    filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                                b.coeff[0], b.coeff[1], b.coeff[2]};
    return filtercoeff;
}

filter_coeff get2HighPass(const double w0, const double q)
{
    filter_middle_coeff a = solve_a(w0, 0.5 / q, 1);
    filter_middle_coeff A = get_AB(a);
    filter_middle_coeff phi0 = get_phi(w0);

    filter_middle_coeff b={0};
    b.coeff[0] = q * sqrt(dot_product(A, phi0)) / 4 / phi0.coeff[1];
    b.coeff[1] = -2 * b.coeff[0];
    b.coeff[2] = b.coeff[0];
    // return {a[0], a[1], a[2], b[0], b[1], b[2]};

    filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                                b.coeff[0], b.coeff[1], b.coeff[2]};
    return filtercoeff;
}

filter_coeff get2BandPass(const double w0, double q)
{
    q = max(q, 0.025);
    filter_middle_coeff a = solve_a(w0, 0.5 / q , -1);
    filter_middle_coeff A = get_AB(a);

    if (w0 > pi / 32) {
        filter_middle_coeff phi0 = get_phi(w0);
        double R1 = dot_product(phi0, A);
        filter_middle_coeff RR2 ={-1, 1, 4 * (phi0.coeff[0] - phi0.coeff[1])};
        double R2 = dot_product(RR2, A);

        filter_middle_coeff B={0};
        B.coeff[0] = 0.0;
        B.coeff[2] = (R1 - R2 * phi0.coeff[1]) / 4 / pow(phi0.coeff[1], 2);
        B.coeff[1] = R2 + 4 * (phi0.coeff[1] - phi0.coeff[0]) * B.coeff[2];

        const filter_middle_coeff b = get_ab(B);
        // return {a[0], a[1], a[2], b[0], b[1], b[2]};

        filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                                    b.coeff[0], b.coeff[1], b.coeff[2]};
        return filtercoeff;

    } else {
        const bandwidth_coeff _w = get_bandwidth(w0, q);
        const double w1 = _w.coeff[0], w2 = _w.coeff[1];
        filter_middle_coeff ws={{0, w0, w0 > piHalf ? w1 : w2}};
        const filter_middle_coeff _ws = ws;
        filter_middle_coeff B={{-1, -1, -1}};
        uint32_t trial = 0;
        while (!check_AB(B) && trial < 20) {
            trial += 1;
            filter_middle_coeff phi[3]={0};
            filter_middle_coeff res={0};
            for (size_t i = 0; i < 3; ++i) {
                phi[i] = get_phi(ws.coeff[i]);
                res.coeff[i] = get2BandPassMagnitude2(w0, q, ws.coeff[i]) * dot_product(phi[i], A);
            }
            B = linear_solve(phi, res);
            ws.coeff[2] = w0 > piHalf ? 0.9 * ws.coeff[2] : 0.9 * ws.coeff[2] + 0.1 * pi;
        }
        if (trial == 20) {
            ws = _ws;
            filter_middle_coeff phi[3]={0};
            filter_middle_coeff res={0};
            for (size_t i = 0; i < 3; ++i) {
                phi[i] = get_phi(ws.coeff[i]);
                res.coeff[i] = get2BandPassMagnitude2(w0, q, ws.coeff[i]) * dot_product(phi[i], A);
            }
            B = linear_solve(phi, res);
            ws.coeff[2] = w0 > piHalf ? 0.9 * ws.coeff[2] : 0.9 * ws.coeff[2] + 0.1 * pi;
        }
        const filter_middle_coeff b = get_ab(B);
        // return {a[0], a[1], a[2], b[0], b[1], b[2]};

        filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                            b.coeff[0], b.coeff[1], b.coeff[2]};
        return filtercoeff;
    }
}

filter_coeff get2Notch(const double w0, const double q)
{

    filter_middle_coeff b;
    if (w0 < pi) {
       // filter_middle_coeff b = {1, -2 * cos(w0), 1};
        b.coeff[0]=1;
        b.coeff[1]=-2 * cos(w0);
        b.coeff[1]=1;
    } else {
        // filter_middle_coeff b = {1, -2 * sinh(w0), 1};
        b.coeff[0]=1;
        b.coeff[1]=-2 * sinh(w0);
        b.coeff[1]=1;
    }
    filter_middle_coeff B = get_AB(b);

    const bandwidth_coeff _w = get_bandwidth(w0, q);
    const double w1 = _w.coeff[0], w2 = _w.coeff[1];
    const filter_middle_coeff ws = {0, w1, w2 < pi ? w2 : 0.5 * (w0 + w1)};

    filter_middle_coeff phi[3]={0};
    filter_middle_coeff res={0};
    for (size_t i = 0; i < 3; ++i)
    {
        phi[i] = get_phi(ws.coeff[i]);
        res.coeff[i] = dot_product(phi[i], B) / get2NotchMagnitude2(w0, q, ws.coeff[i]);
    }

    const filter_middle_coeff A = linear_solve(phi, res);
    const filter_middle_coeff a = get_ab(A);
    // return {a[0], a[1], a[2], b[0], b[1], b[2]};

    filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                                    b.coeff[0], b.coeff[1], b.coeff[2]};
    return filtercoeff;
}

filter_coeff get2Peak(double w0, double g, double q)
{
    const filter_middle_coeff a = solve_a(w0, 0.5 / sqrt(g) / q ,-1);
    const filter_middle_coeff A = get_AB(a);
    const filter_middle_coeff phi0 = get_phi(w0);

    const double R1 = dot_product(A, phi0) * pow(g, 2);
    const double R2 = (-A.coeff[0] + A.coeff[1] + 4 * (phi0.coeff[0] - phi0.coeff[1]) * A.coeff[2]) * pow(g, 2);

    filter_middle_coeff B= {{A.coeff[0], 0, 0}};
    B.coeff[2] = (R1 - R2 * phi0.coeff[1] - B.coeff[0]) / (4 * pow(phi0.coeff[1], 2));
    B.coeff[1] = R2 + B.coeff[0] + 4 * (phi0.coeff[1] - phi0.coeff[0]) * B.coeff[2];
    const filter_middle_coeff b = get_ab(B);

    // return {a[0], a[1], a[2], b[0], b[1], b[2]};
    filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                                b.coeff[0], b.coeff[1], b.coeff[2]};
    return filtercoeff;
}

filter_coeff get2TiltShelf(const double w0, double g, const double q)
{
    const bool reverse_ab = (g > 1);
    if (g > 1) {
        g = 1 / g;
    }
    const double g_sqrt = sqrt(g);
    const filter_middle_coeff a = solve_a(w0, sqrt(g_sqrt) / 2 / q, sqrt(g_sqrt));
    const filter_middle_coeff A = get_AB(a);

    const double c2 = g_sqrt * (-1 + 2 * q * q);
    const double c0 = c2 * pow(w0, 4);
    const double c1 = -2 * (1 + g) * pow(q * w0, 2);
    double delta = c1 * c1 - 4 * c0 * c2;
    filter_middle_coeff ws={0};
    if (delta <= 0)
    {
        // ws = {0, w0 / 2, w0};
        ws.coeff[0]=0;
        ws.coeff[1]=w0 / 2;
        ws.coeff[2] = w0;
    }
    else
    {
        delta = sqrt(delta);
        const double sol1 = (-c1 + delta) / 2 / c2;
        const double sol2 = (-c1 - delta) / 2 / c2;
        if (sol1 < 0 || sol2 < 0) {
            ws.coeff[0]=0;
            ws.coeff[1]=w0 / 2;
            ws.coeff[2] = w0;
        } else {
            const double w1 = sqrt((-c1 + delta) / 2 / c2);
            const double w2 = sqrt((-c1 - delta) / 2 / c2);
            if (w1 < pi || w2 < pi) {
                // ws = {0, min(w1, w2), min(max(w1, w2), pi)};
                ws.coeff[0] = 0;
                ws.coeff[1] = min(w1, w2);
                ws.coeff[2] = min(max(w1, w2), pi);
            } else {
                // ws = {0, piHalf, pi};
                ws.coeff[0] = 0;
                ws.coeff[1] = piHalf;
                ws.coeff[2] = pi;
            }
        }
    }
    filter_middle_coeff B = {-1, -1, -1};
    uint32_t trial = 0;
    const filter_middle_coeff _ws = ws;
    while (!check_AB(B) && trial < 20) {
        trial += 1;
        filter_middle_coeff phi[3]={0};
        filter_middle_coeff res={0};
        for (size_t i = 0; i < 3; ++i) {
            phi[i] = get_phi(ws.coeff[i]);
            res.coeff[i] = get2TiltShelfMagnitude2(w0, g, q, ws.coeff[i]) * dot_product(phi[i], A);
        }
        B = linear_solve(phi, res);
        ws.coeff[2] = 0.5 * (ws.coeff[2] + pi);
    }
    if (trial == 20) {
        ws = _ws;
        filter_middle_coeff phi[3]={0};
        filter_middle_coeff res={0};
        for (size_t i = 0; i < 3; ++i) {
            phi[i] = get_phi(ws.coeff[i]);
            res.coeff[i] = get2TiltShelfMagnitude2(w0, g, q, ws.coeff[i]) * dot_product(phi[i], A);
        }
        B = linear_solve(phi, res);
        ws.coeff[2] = w0 > piHalf ? 0.9 * ws.coeff[2] : 0.9 * ws.coeff[2] + 0.1 * pi;
    }
    const filter_middle_coeff b = get_ab(B);
    if (reverse_ab) {
        // return {b[0], b[1], b[2], a[0], a[1], a[2]};
        filter_coeff filtercoeff = {b.coeff[0], b.coeff[1], b.coeff[2],
                                    a.coeff[0], a.coeff[1], a.coeff[2]};

        return filtercoeff;
    } else {
        filter_coeff filtercoeff = {a.coeff[0], a.coeff[1], a.coeff[2],
                                    b.coeff[0], b.coeff[1], b.coeff[2]};
        return filtercoeff;
    }
}

filter_coeff get2LowShelf(const double w0, const double g, const double q)
{
    const filter_coeff ab = get2TiltShelf(w0, 1 / g, q);
    const double A = sqrt(g);
    // return {ab[0], ab[1], ab[2], ab[3] * A, ab[4] * A, ab[5] * A};
    filter_coeff filtercoeff ={ab.coeff[0], ab.coeff[1], ab.coeff[2],
                            ab.coeff[3] * A, ab.coeff[4] * A, ab.coeff[5] * A};
    return filtercoeff;
}

filter_coeff get2HighShelf(const double w0, const double g, const double q)
{
    const filter_coeff ab = get2TiltShelf(w0, g, q);
    const double A = sqrt(g);
    // return {ab[0], ab[1], ab[2], ab[3] * A, ab[4] * A, ab[5] * A};
    filter_coeff filtercoeff ={ab.coeff[0], ab.coeff[1], ab.coeff[2],
                            ab.coeff[3] * A, ab.coeff[4] * A, ab.coeff[5] * A};
    return filtercoeff;
}

filter_middle_coeff solve_a(const double w0, const double b, const double c)
{
    filter_middle_coeff a;
    a.coeff[0] = 1.0;
    if (b <= c) {
        a.coeff[1] = -2 * exp(-b * w0) * cos(sqrt(c * c - b * b) * w0);
    } else {
        a.coeff[1] = -2 * exp(-b * w0) * cosh(sqrt(b * b - c * c) * w0);
    }
    a.coeff[2] = exp(-2 * b * w0);
    return a;
}

filter_middle_coeff get_AB(filter_middle_coeff a)
{
    filter_middle_coeff A={0};
    A.coeff[0] = pow(a.coeff[0] + a.coeff[1] + a.coeff[2], 2);
    A.coeff[1] = pow(a.coeff[0] - a.coeff[1] + a.coeff[2], 2);
    A.coeff[2] = -4 * a.coeff[2];
    return A;
}

uint8_t check_AB(filter_middle_coeff A)
{
    return A.coeff[0] > 0 && A.coeff[1] > 0 && pow(0.5 * (sqrt(A.coeff[0]) + sqrt(A.coeff[1])), 2) + A.coeff[2] > 0;
}

filter_middle_coeff get_ab(filter_middle_coeff A)
{
    filter_middle_coeff a = {0};
    const double A0 = sqrt(max(A.coeff[0], 0.0));
    const double A1 = sqrt(max(A.coeff[1], 0.0));
    double W = 0.5 * (A0 + A1);
    double temp = max(W * W + A.coeff[2], 0.0);
    a.coeff[0] = 0.5 * (W + sqrt(temp));
    a.coeff[1] = 0.5 * (A0 - A1);
    a.coeff[2] = -A.coeff[2] / 4 / a.coeff[0];
    return a;
}

filter_middle_coeff get_phi(const double w)
{
    filter_middle_coeff phi={0};
    phi.coeff[0] = 1 - pow(sin(w / 2), 2);
    phi.coeff[1] = 1 - phi.coeff[0];
    phi.coeff[2] = 4 * phi.coeff[0] * phi.coeff[1];
    return phi;
}

filter_middle_coeff linear_solve(const filter_middle_coeff A[3],const filter_middle_coeff b)
{
    filter_middle_coeff x={0};
    if (fabsf(A[0].coeff[0]) > fabsf(A[0].coeff[1])) {
        x.coeff[0] = b.coeff[0] / A[0].coeff[0];
        double denominator = -(A[1].coeff[2] * A[2].coeff[1] - A[1].coeff[1] * A[2].coeff[2]);
        x.coeff[1] = A[2].coeff[2] * b.coeff[1] - A[1].coeff[2] * b.coeff[2] + A[1].coeff[2] * A[2].coeff[0] * x.coeff[0] - A[1].coeff[0] * A[2].coeff[2] * x.coeff[0];
        x.coeff[1] /= denominator;
        x.coeff[2] = -A[2].coeff[1] * b.coeff[1] + A[1].coeff[1] * b.coeff[2] - A[1].coeff[1] * A[2].coeff[0] * x.coeff[0] + A[1].coeff[0] * A[2].coeff[1] * x.coeff[0];
        x.coeff[2] /= denominator;
    } else {
        x.coeff[1] = b.coeff[0] / A[0].coeff[1];
        double denominator = -(A[1].coeff[2] * A[2].coeff[0] - A[1].coeff[0] * A[2].coeff[2]);
        x.coeff[0] = A[1].coeff[2] * A[2].coeff[1] * b.coeff[0] - A[1].coeff[1] * A[2].coeff[2] * b.coeff[0] + A[2].coeff[2] * b.coeff[1] - A[1].coeff[2] * b.coeff[2];
        x.coeff[0] /= denominator;
        x.coeff[2] = A[1].coeff[1] * A[2].coeff[0] * b.coeff[0] - A[1].coeff[0] * A[2].coeff[1] * b.coeff[0] - A[2].coeff[0] * b.coeff[1] + A[1].coeff[0] * b.coeff[2];
        x.coeff[2] /= denominator;
    }
    return x;
}

