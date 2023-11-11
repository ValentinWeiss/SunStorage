import { FC, useState, useEffect } from "react";
import { useLocation } from "react-router-dom";

import { Box, Toolbar } from "@mui/material";

import { RequiredChildrenProps } from "../utils";

import LayoutDrawer from "./LayoutDrawer";
import LayoutAppBar from "./LayoutAppBar";
import { LayoutContext } from "./context";

export const DRAWER_WIDTH = 240;

const Layout: FC<RequiredChildrenProps> = ({ children }) => {
    const [mobileOpen, setMobileOpen] = useState(false);
    const [title, setTitle] = useState("SunStorage");
    const { pathname } = useLocation();

    const handleDrawerToggle = () => {
        setMobileOpen(!mobileOpen);
    };

    useEffect(() => setMobileOpen(false), [pathname]);

    return (
        <LayoutContext.Provider value={{ title, setTitle }}>
            <LayoutAppBar title={title} onToggleDrawer={handleDrawerToggle} />
            <LayoutDrawer
                mobileOpen={mobileOpen}
                onClose={handleDrawerToggle}
            />
            <Box
                component="main"
                className="page-host"
                sx={{
                    marginLeft: { md: `${DRAWER_WIDTH}px` },
                    height: "100vh",
                    display: "flex",
                    flexDirection: "column",
                    alignItems: "stretch",
                }}
            >
                <Toolbar />
                {children}
            </Box>
        </LayoutContext.Provider>
    );
};

export default Layout;
