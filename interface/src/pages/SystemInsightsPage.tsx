import { Box, Button, Switch, Typography } from "@mui/material";
import { useEffect, useState } from "react";
import { ESP32FetchClient } from "../api/http";
import { databaseInformation, databaseInformationAllParams } from "../data";
import { useFetch, useFetchParam } from "../hooks";
import { useTitle } from "../layouts";
import { getDateFromYYYYMMDDHHmmss, handleFetchError } from "../utils";
import { MobileDateTimePicker } from "@mui/x-date-pickers";
import { Chart, registerables } from "chart.js";
import zoomPlugin from "chartjs-plugin-zoom";
import {
    ConsumptionWattChart,
    CurrentsChart,
    VoltageBatteryChart,
} from "../components";

Chart.register(...registerables);
Chart.register(zoomPlugin);

export function SystemInsightsPage() {
    useTitle("System Insights");

    const [startTime, setStartTime] = useState<Date>(new Date());
    const [endTime, setEndTime] = useState<Date>(new Date());
    const [noEndTime, setNoEndTime] = useState<boolean>(true);
    const [errorText, setErrorText] = useState<string>("");

    const { data: databaseInformation, fetchData: fetchDatabaseInformation } =
        useFetchParam<databaseInformation[], databaseInformationAllParams>(
            ESP32FetchClient.GetDatabaseAll,
            [],
            handleFetchError,
            undefined,
            (json: any): databaseInformation[] => {
                console.log("data from database:");
                console.log(json);

                return json.map((x) => {
                    let data = x as databaseInformation;

                    data.TimestampID = getDateFromYYYYMMDDHHmmss(
                        x.TimestampID
                    );

                    return data;
                })
            }
        );

    useEffect(() => {
        console.log("new database information useEffect");
        console.log(databaseInformation);
    }, [databaseInformation]);

    const loadDbData = () => {
        if (!noEndTime) {
            if (startTime >= endTime) {
                setErrorText("Start Time must be smaller than End Time");
                return;
            }
        }

        setErrorText("");

        // TODO: gui abfragen immer splitten: max x tage pro abfrage und dann öfter abfragen
        const params: databaseInformationAllParams = {
            startTime: startTime,
            endTime: noEndTime ? undefined : endTime,
        };

        fetchDatabaseInformation(params);
    };

    return (
        <>
            <Box sx={{ m: 2 }}>
                <Typography variant="h4">Configure Data to Load:</Typography>

                <Box sx={{ my: 2 }}>
                    <MobileDateTimePicker
                        sx={{ mr: 1 }}
                        format="dd.MM.yyyy HH:mm"
                        value={startTime}
                        ampm={false}
                        label="Start Time"
                        onChange={(e) => e && setStartTime(e)}
                    />
                    <MobileDateTimePicker
                        sx={{ mr: 1 }}
                        format="dd.MM.yyyy HH:mm"
                        value={endTime}
                        ampm={false}
                        label="End Time"
                        onChange={(e) => e && setEndTime(e)}
                        disabled={noEndTime}
                    />
                    <Box sx={{ display: "inline-block" }}>
                        No End Time:
                        <Switch
                            checked={noEndTime}
                            onChange={(e) =>
                                e && setNoEndTime(e.target.checked)
                            }
                        />
                    </Box>
                </Box>
                {errorText && (
                    <Typography color="error">{errorText}</Typography>
                )}
                <Button variant="contained" onClick={loadDbData}>
                    Load Data
                </Button>
            </Box>
            {databaseInformation?.length > 0 && (
                <>
                    <Box
                        sx={{ m: 1, display: "flex", justifyContent: "center" }}
                    >
                        <Box sx={{ width: "85%", maxWidth: "800px" }}>
                            <VoltageBatteryChart
                                databaseInformation={databaseInformation}
                            />
                        </Box>
                    </Box>
                    <Box
                        sx={{ m: 1, display: "flex", justifyContent: "center" }}
                    >
                        <Box sx={{ width: "85%", maxWidth: "800px" }}>
                            <CurrentsChart
                                databaseInformation={databaseInformation}
                            />
                        </Box>
                    </Box>
                    <Box
                        sx={{ m: 1, display: "flex", justifyContent: "center" }}
                    >
                        <Box sx={{ width: "85%", maxWidth: "800px" }}>
                            <ConsumptionWattChart
                                databaseInformation={databaseInformation}
                            />
                        </Box>
                    </Box>
                </>
            )}
        </>
    );
}
