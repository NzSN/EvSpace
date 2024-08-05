export function waitFor(predicate: () => boolean): Promise<void> {
    const waiting = (_, resolve) => {
        if (predicate()) {
            resolve();
        } else {
            setTimeout(waiting, 10);
        }
    };
    return new Promise(waiting);
}

export function unreachable(): never {}
