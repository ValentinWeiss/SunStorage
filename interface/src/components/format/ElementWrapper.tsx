import { Box, Typography, useTheme } from "@mui/material";
import { PropsWithChildren } from "react";

export const ElementWrapper = (
    props: PropsWithChildren<{ header?: string }>
) => {
    const theme = useTheme();
    const defaultPaddingTop = props?.header ? "1rem" : "0";
    return (
        <Box
            sx={{
                margin: "8px",
            }}
        >
            <Box sx={{ paddingTop: defaultPaddingTop }}>
                <Box
                    sx={{
                        border: "1px solid",
                        borderColor: "text.primary", //theme.palette.primary.main
                        borderRadius: "10px",
                        padding: "8px",
                    }}
                >
                    <Box
                        sx={{
                            marginTop: "calc(-1rem - 8px)",
                            marginBottom: "4px",
                        }}
                    >
                        <Typography
                            variant="h5"
                            sx={{
                                paddingLeft: "10px",
                                paddingRight: "10px",
                                display: "inline-block",
                                backgroundColor: theme.palette.background.paper,
                                color: "text.primary", //theme.palette.primary.main
                            }}
                        >
                            {props.header}
                        </Typography>
                    </Box>
                    {props.children}
                </Box>
            </Box>
        </Box>
    );
};
