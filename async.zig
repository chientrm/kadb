const std = @import("std");
const Allocator = std.mem.Allocator;

pub fn main() void {
    _ = async amainWrap();

    // Typically we would use an event loop to manage resuming async functions,
    // but in this example we hard code what the event loop would do,
    // to make things deterministic.
    resume global_file_frame;
    resume global_download_frame;
}

fn amainWrap() void {
    amain() catch |e| {
        std.debug.print("{}\n", .{e});
        if (@errorReturnTrace()) |trace| {
            std.debug.dumpStackTrace(trace.*);
        }
        std.process.exit(1);
    };
}

fn amain() !void {
    const allocator = std.heap.page_allocator;
    var download_frame = async fetchUrl(allocator, "https://example.com/");
    var awaited_download_frame = false;
    errdefer if (!awaited_download_frame) {
        if (await download_frame) |r| allocator.free(r) else |_| {}
    };

    var file_frame = async readFile(allocator, "something.txt");
    var awaited_file_frame = false;
    errdefer if (!awaited_file_frame) {
        if (await file_frame) |r| allocator.free(r) else |_| {}
    };

    awaited_file_frame = true;
    const file_text = try await file_frame;
    defer allocator.free(file_text);

    awaited_download_frame = true;
    const download_text = try await download_frame;
    defer allocator.free(download_text);

    std.debug.print("download_text: {s}\n", .{download_text});
    std.debug.print("file_text: {s}\n", .{file_text});
}

var global_download_frame: anyframe = undefined;
fn fetchUrl(allocator: Allocator, url: []const u8) ![]u8 {
    _ = url; // this is just an example, we don't actually do it!
    const result = try allocator.dupe(u8, "this is the downloaded url contents");
    errdefer allocator.free(result);
    suspend {
        global_download_frame = @frame();
    }
    std.debug.print("fetchUrl returning\n", .{});
    return result;
}

var global_file_frame: anyframe = undefined;

fn readFile(allocator: Allocator, filename: []const u8) ![]u8 {
    _ = filename; // this is just an example, we don't actually do it!
    const result = try allocator.dupe(u8, "this is the file contents");
    errdefer allocator.free(result);
    suspend {
        global_file_frame = @frame();
    }
    std.debug.print("readFile returning\n", .{});
    return result;
}
