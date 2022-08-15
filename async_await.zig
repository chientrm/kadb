const std = @import("std");
const expect = std.testing.expect;

test "async and await" {
    // The test block is not async and so cannot have a suspend
    // point in it. By using the nosuspend keyword, we promise that
    // the code in amain will finish executing without suspending
    // back to the test block.
    nosuspend amain();
}

fn amain() void {
    var frame = async func();
    comptime try expect(@TypeOf(frame) == @Frame(func));

    const ptr: anyframe->void = &frame;
    const any_ptr: anyframe = ptr;

    resume any_ptr;
    await ptr;
}

fn func() void {
    suspend {}
}
