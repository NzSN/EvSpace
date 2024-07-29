
export function protoPath(msgType: string): string | null {
    switch (msgType) {
        case "Counter":
            return "src/eval/async/messages/counter.proto";
    }

    return null;
}
