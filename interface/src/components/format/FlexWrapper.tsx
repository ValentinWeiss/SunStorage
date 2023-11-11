import { Box, SxProps, useTheme } from "@mui/material";
import { PropsWithChildren } from "react";

export const FlexWrapper = (props: PropsWithChildren<{ sx?: SxProps }>) => {
    const theme = useTheme();

    return (
        <Box
            sx={{
                gap: "10px",
                display: "flex",
                flexWrap: "wrap",
                flexDirection: "row",
                ...props.sx,
            }}
        >
            {props.children}
        </Box>
    );
};
