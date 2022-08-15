const std = @import("std");
const expect = std.testing.expect;

test "async fn pointer in a struct field" {
    var data: i32 = 1;
    const Foo = struct {
        bar: fn (*i32) callconv(.Async) void,
    };
    var foo = Foo{ .bar = func };
    var bytes: [64]u8 align(@alignOf(@Frame(func))) = undefined;
    const f = @asyncCall(&bytes, {}, foo.bar, .{&data});
    try expect(data == 2);
    resume f;
    try expect(data == 4);
}

fn func(y: *i32) void {
    defer y.* += 2;
    y.* += 1;
    suspend {}
}
