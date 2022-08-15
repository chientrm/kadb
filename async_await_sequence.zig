const std = @import("std");
const expect = std.testing.expect;

var the_frame: anyframe = undefined;
var final_result: i32 = 0;

test "async function await" {
    seq('a');
    _ = async amain();
    seq('f');
    resume the_frame;
    seq('i');
    try expect(final_result == 1234);
    try expect(std.mem.eql(u8, &seq_points, "abcdefghi"));
}
fn amain() void {
    seq('b');
    var f = async another();
    seq('e');
    final_result = await f;
    seq('h');
}
fn another() i32 {
    seq('c');
    suspend {
        seq('d');
        the_frame = @frame();
    }
    seq('g');
    return 1234;
}

var seq_points = [_]u8{0} ** "abcdefghi".len;
var seq_index: usize = 0;

fn seq(c: u8) void {
    seq_points[seq_index] = c;
    seq_index += 1;
}
