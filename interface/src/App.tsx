import React, { createRef, RefObject } from "react";
import "./App.scss";
import { AppRouting } from "./AppRouting";
import { SnackbarProvider } from "notistack";
import { IconButton } from "@mui/material";
import CloseIcon from "@mui/icons-material/Close";
import { Layout } from "./layouts";
import Theme from "./Theme";
import { AttachGlobalContexts } from "./attachGlobalContexts";

function App() {
    const notistackRef: RefObject<any> = createRef();

    const onClickDismiss = (key: string | number | undefined) => () => {
        notistackRef.current.closeSnackbar(key);
    };

    return (
        <Theme>
            <SnackbarProvider
                maxSnack={3}
                anchorOrigin={{ vertical: "bottom", horizontal: "left" }}
                ref={notistackRef}
                action={(key) => (
                    <IconButton onClick={onClickDismiss(key)} size="small">
                        <CloseIcon />
                    </IconButton>
                )}
            >
                <AttachGlobalContexts>
                    <Layout>
                        <AppRouting />
                    </Layout>
                </AttachGlobalContexts>
            </SnackbarProvider>
        </Theme>
    );
}

export default App;
