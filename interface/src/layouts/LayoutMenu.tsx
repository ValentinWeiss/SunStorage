import { FC } from "react";

import { List } from "@mui/material";

import LayoutMenuItem from "./LayoutMenuItem";
import { AvailableRoutes } from "../AppRouting";

const LayoutMenu: FC = () => {
    return (
        <>
            <List disablePadding component="nav">
                {AvailableRoutes.map((x) => {
                    return (
                        <LayoutMenuItem
                            key={x.href}
                            icon={x.icon}
                            label={x.label}
                            to={x.href}
                        />
                    );
                })}
            </List>
        </>
    );
};

export default LayoutMenu;
