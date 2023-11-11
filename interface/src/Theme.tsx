import { FC } from "react";

import { CssBaseline } from "@mui/material";
import {
    createTheme,
    responsiveFontSizes,
    ThemeProvider,
} from "@mui/material/styles";
import {
    blueGrey,
    purple,
    red,
    yellow,
    orange,
    green,
} from "@mui/material/colors";

import { RequiredChildrenProps } from "./utils";

// https://mui.com/material-ui/customization/dark-mode/
const themeDark = responsiveFontSizes(
    createTheme({
        typography: {
            fontSize: 13,
        },
        palette: {
            mode: "dark",
            primary: {
                main: "#e8da1c", //yellow[700],
            },
            secondary: {
                main: "#5050e6", // purple[500],
            },
            error: {
                main: red[700],
            },
            warning: {
                main: orange[600],
            },
            info: {
                main: blueGrey[400],
            },
            success: {
                main: green[700],
            },
        },
    })
);

themeDark.components = {
    ...themeDark.components,
    MuiListItem: {
        styleOverrides: {
            root: {
                "&.Mui-selected": {
                    backgroundColor: themeDark.palette.primary.main,
                    color: themeDark.palette.primary.contrastText,
                },
            },
        },
    },
    MuiMenuItem: {
        styleOverrides: {
            root: {
                //backgroundColor: themeDark.palette.background.paper[10],
                "&.Mui-selected:hover, &:hover": {
                    backgroundColor: themeDark.palette.primary.main,
                    color: themeDark.palette.primary.contrastText,
                },
            },
        },
    },
    MuiButton: {
        styleOverrides: {
            root: {
                minWidth: "125px",
            },
        },
    },
    MuiInputBase: {
        styleOverrides: {
            root: {
                minWidth: "125px",
            },
        },
    },
};

const Theme: FC<RequiredChildrenProps> = ({ children }) => (
    <ThemeProvider theme={themeDark}>
        <CssBaseline />
        {children}
    </ThemeProvider>
);

export default Theme;
