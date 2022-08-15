const print = @import("std").debug.print;
const mem = @import("std").mem; // will be used to compare bytes

pub fn main() void {
    const bytes = "hello";
    print("{s}\n", .{@typeName(@TypeOf(bytes))}); // *const [5:0]u8
    print("{d}\n", .{bytes.len}); // 5
    print("{c}\n", .{bytes[1]}); // 'e'
    print("{d}\n", .{bytes[5]}); // 0
    print("{}\n", .{'e' == '\x65'}); // true
    print("{d}\n", .{'\u{1f4a9}'}); // 128169
    print("{d}\n", .{'💯'}); // 128175
    print("{}\n", .{mem.eql(u8, "hello", "h\x65llo")}); // true
    print("0x{x}\n", .{"\xff"[0]}); // non-UTF-8 strings are possible with \xNN notation.
    print("{u}\n", .{'⚡'});
}
