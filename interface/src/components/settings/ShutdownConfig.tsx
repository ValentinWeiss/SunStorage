import { Box, Button, TextField, Typography } from "@mui/material";
import { useContext, useState } from "react";
import { useForm, Controller } from "react-hook-form";
import { yupResolver } from "@hookform/resolvers/yup";
import { ElementWrapper, FlexWrapper } from "../";
import {
    ESPStateData,
    shutdownConfiguration,
    shutdownConfigurationValidationSchema,
} from "../../data";
import { ESP32FetchClient } from "../../api/http";
import { handleFetchError, httpResponseThrowIfInvalid } from "../../utils";

export function ShutdownConfig() {
    const {
        setValue,
        handleSubmit,
        control,
        formState: { errors },
    } = useForm<shutdownConfiguration>({
        resolver: yupResolver(shutdownConfigurationValidationSchema),
        defaultValues: {
            dcf77: 0,
            gps: 0,
            http: 0,
            wifi: 0,
        },
    });

    const onSubmit = async (data: shutdownConfiguration) => {
        console.log(data);
        var result = await ESP32FetchClient.Shutdown(data)
            .then((res) => httpResponseThrowIfInvalid(res))
            .catch((x) => handleFetchError(x));
        console.log(result);
    };

    return (
        <>
            <ElementWrapper header="Shutdown Items">
                <form onSubmit={handleSubmit(onSubmit)}>
                    <Controller
                        control={control}
                        name="http"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Http: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.http}
                                            helperText={errors?.http?.message}
                                            type="number"
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="gps"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Gps: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.gps}
                                            helperText={errors?.gps?.message}
                                            type="number"
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="wifi"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Wifi: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.wifi}
                                            helperText={errors?.wifi?.message}
                                            type="number"
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <Controller
                        control={control}
                        name="dcf77"
                        render={({ field }) => (
                            <table>
                                <tr>
                                    <td>Dcf77: </td>
                                    <td>
                                        <TextField
                                            error={!!errors?.dcf77}
                                            helperText={errors?.dcf77?.message}
                                            type="number"
                                            {...field}
                                        />
                                    </td>
                                </tr>
                            </table>
                        )}
                    />
                    <FlexWrapper
                        sx={{
                            flexDirection: "row-reverse",
                            marginTop: "10px",
                        }}
                    >
                        <Button
                            variant="contained"
                            color="primary"
                            type="submit"
                        >
                            Shutdown
                        </Button>
                    </FlexWrapper>
                </form>
            </ElementWrapper>
        </>
    );
}
