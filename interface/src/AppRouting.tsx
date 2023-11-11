import { Route, Routes } from "react-router-dom";
import {
    MqttLibTest,
    MqttTestGui,
    ThemeTest,
    HttpTestPage,
    Simulation,
    SubMenuSimulation,
    SystemSettingsPage,
    GeneralSettingsPage,
    SystemInfoPage,
    Logger,
} from "./pages";
import { Web } from "@mui/icons-material";
import InfoIcon from "@mui/icons-material/Info";
import SettingsIcon from "@mui/icons-material/Settings";
import PublicIcon from "@mui/icons-material/Public";
import { SystemInsightsPage } from "./pages/SystemInsightsPage";
import AutoGraphIcon from "@mui/icons-material/AutoGraph";

export const AvailableRoutes = [
    // {
    //     label: "ThemeTest",
    //     href: "/themeTest",
    //     icon: Web,
    //     element: <ThemeTest />,
    // },
    // {
    //     label: "HttpTestPage",
    //     href: "/httpTestPage",
    //     icon: Web,
    //     element: <HttpTestPage />,
    // },
    // {
    //     label: "mqtttestgui",
    //     href: "/mqtttestgui",
    //     icon: Web,
    //     element: <MqttTestGui />,
    // },
    // {
    //     label: "mqttlibtest",
    //     href: "/mqttlibtest",
    //     icon: Web,
    //     element: <MqttLibTest />,
    // },
    // {
    //     label: "Logger",
    //     href: "/logger",
    //     icon: Web,
    //     element: <Logger />,
    // },
    {
        label: "System Insights",
        href: "/systeminsights",
        icon: AutoGraphIcon,
        element: <SystemInsightsPage />,
    },
    {
        label: "Simulation",
        href: "/simulation",
        icon: PublicIcon,
        element: <Simulation />,
        menuElement: <SubMenuSimulation />,
    },
    {
        label: "System Info",
        href: "/systeminfo",
        icon: InfoIcon,
        element: <SystemInfoPage />,
    },
    {
        label: "General Settings",
        href: "/generalsettings",
        icon: SettingsIcon,
        element: <GeneralSettingsPage />,
    },
    {
        label: "System Settings",
        href: "/systemsettings",
        icon: SettingsIcon,
        element: <SystemSettingsPage />,
    },
];
export const AppRouting = () => {
    return (
        <Routes>
            {AvailableRoutes.map((x) => {
                return <Route key={x.href} path={x.href} element={x.element} />;
            })}
        </Routes>
    );
};
