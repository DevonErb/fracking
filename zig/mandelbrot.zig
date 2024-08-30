//zig mandelbrot
const std = @import("std");

pub fn mandelbrot(w: usize, h: usize, mi: u8) void {
    const stdout = std.io.getStdOut().writer();
    const x_min: f64 = -2.5;
    const x_max: f64 = 1.0;
    const y_min: f64 = -1.0;
    const y_max: f64 = 1.0;

    var y: usize = 0;
    var x: usize = 0;

    while (y < h) {
        while (x < w) {
            const c_re: f64 = @intToFloat(f64, x) / @intToFloat(f64, width) * (x_max - x_min) + x_min;
            const c_im: f64 = @intToFloat(f64, y) / @intToFloat(f64, height) * (y_max - y_min) + y_min;
            var z_re: f64 = c_re;
            var z_im: f64 = c_im;
            var is_inside: bool = true;
            var iter: u8 = 0;

            while (iter < max_iter) {
                const z_re2 = z_re * z_re;
                const z_im2 = z_im * z_im;

                if (z_re2 + z_im2 > 4.0) {
                    is_inside = false;
                    break;
                }

                const new_z_re = z_re2 - z_im2 + c_re;
                z_im = 2.0 * z_re * z_im + c_im;
                z_re = new_z_re;
                iter += 1;
            }

            if (is_inside) {
                try stdout.print("*", .{});
            } else {
                try stdout.print(" ", .{});
            }
        }
        try stdout.print("\n", .{});
    }
}

pub fn main() void {
    std.debug.print("Mandelbrot \n", .{});
    const width: usize = 80;
    const height: usize = 40;
    const max_iterations: u8 = 100;

    mandelbrot(width, height, max_iterations);

    return;
}
