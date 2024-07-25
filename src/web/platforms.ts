export enum Platform {
    AUTO,
    WASM,
    NATIVE,
}

export function is_nodejs() {
    return (typeof window === 'undefined');
}

export function is_browser() {
    return !is_nodejs();
}
