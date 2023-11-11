import { SelectChangeEvent } from "@mui/material";

// react form events
export const handleChangeObj = <T>(
    e:
        | React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>
        | SelectChangeEvent<any>,
    dispatcher: React.Dispatch<React.SetStateAction<T>>
): void => {
    dispatcher((prevState) => ({
        ...prevState,
        [e.target.name]: e.target.value,
    }));
};

export const handleChangeVal = <T>(
    e:
        | React.ChangeEvent<HTMLInputElement | HTMLTextAreaElement>
        | SelectChangeEvent<any>,
    dispatcher: React.Dispatch<React.SetStateAction<T>>
): void => {
    dispatcher(e.target.value);
};
