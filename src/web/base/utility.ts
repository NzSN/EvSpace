export function waitFor(predicate: () => boolean): Promise<void> {
    const waiting = (resolve) => {
        if (predicate()) {
            resolve();
        } else {
            setTimeout(_ => waiting(resolve), 1);
        }
    };
    return new Promise(waiting);
}
