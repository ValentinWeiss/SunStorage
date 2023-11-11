export const Activator = <T extends {}>(type: new () => T) => {
    return type;
};

export const Cast = <T extends {}>(
    type: new () => T,
    obj: any
): T | undefined => {
    if (obj == null) {
        return undefined;
    }
    let a: T | undefined = new type();
    Object.assign(a, obj);
    return a;
};

export const CastArray = <T extends {}>(
    type: new () => T,
    obj: any
): (T | undefined)[] => {
    let a: (T | undefined)[] = [];
    obj.forEach((item: any) => {
        a.push(Cast(type, item));
    });
    return a;
};
