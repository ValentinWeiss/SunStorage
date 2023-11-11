import { FC, useEffect, useRef, useState } from "react";

import {
    Box,
    Divider,
    Drawer,
    Toolbar,
    Typography,
    styled,
    IconButton,
} from "@mui/material";

import LayoutMenu from "./LayoutMenu";
import { DRAWER_WIDTH } from "./Layout";
import { Route, Routes, useLocation } from "react-router-dom";
import { AvailableRoutes } from "../AppRouting";

import ArrowBackIcon from "@mui/icons-material/ArrowBack";
import ArrowForwardIcon from "@mui/icons-material/ArrowForward";

const LayoutDrawerLogo = styled("img")(({ theme }) => ({
    [theme.breakpoints.down("sm")]: {
        height: 24,
        marginRight: theme.spacing(2),
    },
    [theme.breakpoints.up("sm")]: {
        height: 36,
        marginRight: theme.spacing(2),
    },
}));

interface LayoutDrawerProps {
    mobileOpen: boolean;
    onClose: () => void;
}

const LayoutDrawer: FC<LayoutDrawerProps> = ({ mobileOpen, onClose }) => {
    const location = useLocation();
    const previousLocation = useRef(location);
    const [subMenu, setSubMenu] = useState<JSX.Element>();
    const [subMenuOpen, setSubMenuOpen] = useState(false);

    const SetSubMenuState = () => {
        const element = AvailableRoutes.find(
            (x) => x.href === location.pathname
        )?.menuElement;
        setSubMenu(element);
        setSubMenuOpen(!!element);
    };

    useEffect(() => {
        if (location !== previousLocation.current) {
            SetSubMenuState();
        }
        previousLocation.current = location;
    }, [location]);

    useEffect(() => {
        SetSubMenuState();
    }, []);

    const drawer = (
        <>
            <Toolbar disableGutters>
                <Box display="flex" alignItems="center" px={2}>
                    <LayoutDrawerLogo src="favicon.ico" alt="SunStorage" />
                    <Typography variant="h6" color="textPrimary">
                        SunStorage
                    </Typography>
                </Box>
                <Divider absolute />
            </Toolbar>
            <Divider absolute />
            {!!subMenu && (
                <Box
                    sx={{
                        display: "flex",
                        justifyContent: "space-between",
                        padding: "8px 18px",
                    }}
                >
                    <IconButton
                        disabled={!subMenuOpen}
                        onClick={() => setSubMenuOpen(false)}
                    >
                        <ArrowBackIcon />
                    </IconButton>
                    <IconButton
                        disabled={subMenuOpen}
                        onClick={() => setSubMenuOpen(true)}
                    >
                        <ArrowForwardIcon />
                    </IconButton>
                </Box>
            )}
            <Divider />
            {!subMenuOpen && <LayoutMenu />}
            {subMenuOpen && subMenu}
        </>
    );

    return (
        <Box
            component="nav"
            sx={{ width: { md: DRAWER_WIDTH }, flexShrink: { md: 0 } }}
        >
            <Drawer
                variant="temporary"
                open={mobileOpen}
                onClose={onClose}
                ModalProps={{
                    keepMounted: true,
                }}
                sx={{
                    display: { xs: "block", md: "none" },
                    "& .MuiDrawer-paper": {
                        boxSizing: "border-box",
                        width: DRAWER_WIDTH,
                    },
                }}
            >
                {drawer}
            </Drawer>
            <Drawer
                variant="permanent"
                sx={{
                    display: { xs: "none", md: "block" },
                    "& .MuiDrawer-paper": {
                        boxSizing: "border-box",
                        width: DRAWER_WIDTH,
                    },
                }}
                open
            >
                {drawer}
            </Drawer>
        </Box>
    );
};

export default LayoutDrawer;
