import { useRef, useEffect, createContext, useContext } from "react";

export interface LayoutContextValue {
    title: string;
    setTitle: (title: string) => void;
}

const LayoutContextDefaultValue = {} as LayoutContextValue;
export const LayoutContext = createContext(LayoutContextDefaultValue);

export const useTitle = (myTitle: string) => {
    const { title, setTitle } = useContext(LayoutContext);
    const previousTitle = useRef(title);

    const updateTitle = (title: string) => {
        setTitle(title);
        document.title = title;
    };

    useEffect(() => {
        updateTitle(myTitle);
    }, [setTitle, myTitle]);

    useEffect(
        () => () => {
            updateTitle(previousTitle.current);
        },
        [setTitle]
    );
};
