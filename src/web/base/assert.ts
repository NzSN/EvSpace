
// TODO: Remove assert in release build.
export function assert(predicate: () => boolean, message?: string): void {
    if (!predicate()) {
        throw message || "Assertion failed";
    }
}
