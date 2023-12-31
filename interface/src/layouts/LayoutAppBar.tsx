import { FC } from "react";

import { AppBar, IconButton, Toolbar, Typography } from "@mui/material";
import MenuIcon from "@mui/icons-material/Menu";
import { useTheme } from "@mui/material/styles";

export const DRAWER_WIDTH = 240;

interface LayoutAppBarProps {
    title: string;
    onToggleDrawer: () => void;
}

const LayoutAppBar: FC<LayoutAppBarProps> = ({ title, onToggleDrawer }) => {
    const theme = useTheme();

    return (
        <AppBar
            position="fixed"
            sx={{
                width: { md: `calc(100% - ${DRAWER_WIDTH}px)` },
                ml: { md: `${DRAWER_WIDTH}px` },
                boxShadow: "none",
            }}
        >
            <Toolbar
                sx={{
                    backgroundColor: theme.palette.primary.main,
                    color: theme.palette.primary.contrastText,
                }}
            >
                <IconButton
                    color="inherit"
                    aria-label="open drawer"
                    edge="start"
                    onClick={onToggleDrawer}
                    sx={{ mr: 2, display: { md: "none" } }}
                >
                    <MenuIcon />
                </IconButton>
                <Typography variant="h6" noWrap component="div">
                    {title}
                </Typography>
            </Toolbar>
        </AppBar>
    );
};

export default LayoutAppBar;
