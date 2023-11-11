export class ArgumentError extends Error {
    constructor(msg: string) {
        super(msg);
        Object.setPrototypeOf(this, ArgumentError.prototype);
        this.name = "ArgumentError";
    }
}
