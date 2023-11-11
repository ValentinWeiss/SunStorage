import { Box, Button } from "@mui/material";
import { ElementWrapper } from "../";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";
import { ESP32FetchClient } from "../../api/http/clients/ESP32FetchClient";

export function GeneralConfig() {
    const onReclaibrateStateOfChargeClick = async () => {
        var result = await ESP32FetchClient.ReclaibrateStateOfCharge()
            .then((res) => httpResponseThrowIfInvalid(res))
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    const onConfigureTimestampClick = async () => {
        var result = await ESP32FetchClient.SetTimestamp(new Date())
            .then((res) => httpResponseThrowIfInvalid(res))
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    return (
        <>
            <ElementWrapper header="General Config">
                {/* <Box>
                     <Button
                        sx={{ mb: 2 }}
                        variant="contained"
                        color="primary"
                        type="submit"
                        onClick={onConfigureTimestampClick}
                    >
                        Configure Timestamp
                    </Button>
                </Box> */}

                <Box>
                    <Button
                        sx={{ mb: 2 }}
                        variant="contained"
                        color="primary"
                        type="submit"
                        onClick={onReclaibrateStateOfChargeClick}
                    >
                        Reclaibrate State of Charge
                    </Button>
                </Box>
            </ElementWrapper>
        </>
    );
}
