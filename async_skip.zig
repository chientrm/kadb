const std = @import("std");

test "async skip test" {
    var frame = async func();
    const result = await frame;
    try std.testing.expect(result == 1);
}

fn func() i32 {
    suspend {
        resume @frame();
    }
    return 1;
}
