// m13_interop_ffi/ex06_rust_ffi — konvencii Rust FFI: repr(C), Option<Box<T>> -> nullptr.
// Rust-tulkheyna (rustc/cargo) v srede net, poetomu pokazany te zhe konvencii,
// kotorye Rust ispol'zuet na granice: #[repr(C)], extern "C", ownership cherez free.
// Standart: C++23. Zapusk: ./run.ps1 m13_interop_ffi/ex06_rust_ffi
#include <cstdint>
#include <iostream>
#include <print>
#include <type_traits>

extern "C" {

// V Rust: #[repr(C)] struct Widget { id: u32, weight: f64 }
struct Widget {
    std::uint32_t id;
    double weight;
};

// V Rust: #[no_mangle] pub extern "C" fn widget_new(...) -> *mut Widget
// (Option<Box<Widget>> predstavlen kak nullable-ukazatel').
Widget* widget_new(std::uint32_t id, double weight) {
    if (weight < 0.0) {
        return nullptr;  // None
    }
    return new Widget{id, weight};
}

double widget_weight(const Widget* widget) {
    return widget->weight;
}

void widget_free(Widget* widget) {
    delete widget;  // vozvrashcheniye vladeniya v Rust
}
}

int main() {
    static_assert(std::is_standard_layout_v<Widget>, "repr(C) => standard-layout");
    static_assert(std::is_trivially_copyable_v<Widget>, "repr(C) => trivially copyable");
    std::println("repr(C) widget is standard-layout");

    Widget* widget = widget_new(1, 2.5);
    const double weight = widget != nullptr ? widget_weight(widget) : 0.0;
    std::println("widget_new(1, 2.5) -> weight {}", weight);
    widget_free(widget);

    Widget* invalid = widget_new(2, -1.0);
    std::println("widget_new(2, -1) -> {}", invalid != nullptr ? "Some" : "nullptr (None)");
    widget_free(invalid);

    const bool ok = weight == 2.5 && invalid == nullptr;
    std::cout << "rust ffi (simulated conventions) done\n";
    return ok ? 0 : 1;
}
