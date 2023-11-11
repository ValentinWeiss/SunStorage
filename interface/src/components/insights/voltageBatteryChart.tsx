import React, { useState, useEffect, useRef } from "react";
import { Line } from "react-chartjs-2";
import "moment";
import "chartjs-adapter-moment";
import { Chart, ChartData, ChartOptions, Point } from "chart.js";
import { useTheme } from "@mui/material";
import { databaseInformation } from "../../data/databaseInformation";

export function VoltageBatteryChart({
    databaseInformation,
}: {
    databaseInformation: databaseInformation[];
}) {
    const theme = useTheme();
    const chartRef = useRef<Chart | null>(null);
    const [chartData, setChartData] = useState<ChartData<"line"> | null>(null);

    useEffect(() => {
        const chartData: ChartData<"line"> = {
            datasets: [
                {
                    label: "Voltage Higher Cell (mV)",
                    data: [
                        { x: new Date("2023-06-17 09:00").getTime(), y: 12 },
                        { x: new Date("2023-06-17 09:01").getTime(), y: 1 },
                        { x: new Date("2023-06-17 09:02").getTime(), y: 2 },
                        { x: new Date("2023-06-17 09:03").getTime(), y: 3 },
                    ],
                    fill: false,
                    borderColor: theme.palette.primary.main,
                    tension: 0.2,
                },
                {
                    label: "Voltage Lower Cell (mV)",
                    data: [
                        { x: new Date("2023-06-17 09:00").getTime(), y: 15 },
                        { x: new Date("2023-06-17 09:01").getTime(), y: 5 },
                        { x: new Date("2023-06-17 09:02").getTime(), y: 4 },
                        { x: new Date("2023-06-17 09:03").getTime(), y: 8 },
                    ],
                    fill: false,
                    borderColor: theme.palette.secondary.main,
                    tension: 0.2,
                },
                {
                    label: "Voltage Lower Cell (mV)",
                    data: [
                        { x: new Date("2023-06-17 09:00").getTime(), y: 4 },
                        { x: new Date("2023-06-17 09:01").getTime(), y: 7 },
                        { x: new Date("2023-06-17 09:02").getTime(), y: 1 },
                        { x: new Date("2023-06-17 09:03").getTime(), y: 6 },
                    ],
                    fill: false,
                    borderColor: theme.palette.warning.main,
                    tension: 0.2,
                },
            ],
        };

        //setChartData(chartData);
    }, []);

    useEffect(() => {
        if (databaseInformation?.length == 0) return;

        const chartData: ChartData<"line"> = {
            datasets: [
                {
                    label: "Voltage Higher Cell (mV)",
                    data: databaseInformation.map((data) => {
                        if (data?.TimestampID && data.VoltageHigherCell) {
                            const d: Point = {
                                x: +data?.TimestampID,
                                y: +data.VoltageHigherCell,
                            };
                            return d;
                        }
                        return null;
                    }),
                    fill: false,
                    borderColor: theme.palette.primary.main,
                    tension: 0.2,
                },
                {
                    label: "Voltage Lower Cell (mV)",
                    data: databaseInformation.map((data) => {
                        if (data?.TimestampID && data.VoltageLowerCell) {
                            const d: Point = {
                                x: +data?.TimestampID,
                                y: +data.VoltageLowerCell,
                            };
                            return d;
                        }
                        return null;
                    }),
                    fill: false,
                    borderColor: theme.palette.secondary.main,
                    tension: 0.2,
                },
                {
                    label: "Voltage Battery (mV)",
                    data: databaseInformation.map((data) => {
                        if (
                            data?.TimestampID &&
                            data?.VoltageLowerCell &&
                            data?.VoltageHigherCell
                        ) {
                            const d: Point = {
                                x: +data?.TimestampID,
                                y:
                                    +data.VoltageLowerCell +
                                    +data?.VoltageHigherCell,
                            };
                            return d;
                        }
                        return null;
                    }),
                    fill: false,
                    borderColor: theme.palette.warning.main,
                    tension: 0.2,
                },
            ],
        };

        setChartData(chartData);
    }, [databaseInformation]);

    useEffect(() => {
        const handleResize = () => {
            if (chartRef.current) {
                chartRef.current.resize();
            }
        };

        window.addEventListener("resize", handleResize);
        return () => {
            window.removeEventListener("resize", handleResize);
        };
    }, []);

    if (!chartData) {
        return <div>Loading chart data...</div>;
    }

    const data: ChartData<"line", { x: string; y: number }[]> = {
        labels: chartData.labels,
        datasets: chartData.datasets.map((dataset) => ({
            ...dataset,
            data: dataset.data.map((dataPoint) => ({
                x: dataPoint.x,
                y: dataPoint.y,
            })),
        })),
    };

    const options: ChartOptions<"line"> = {
        plugins: {
            zoom: {
                zoom: {
                    wheel: {
                        enabled: true,
                    },
                    drag: {
                        enabled: true,
                    },
                    mode: "x",
                },
            },
        },
        scales: {
            x: {
                type: "time",
                time: {
                    unit: "second",
                    tooltipFormat: "DD.MM.YYYY HH:mm:ss",
                    displayFormats: {
                        //second: "DD.MM.YY HH:mm:ss",
                        second: "HH:mm:ss",
                    },
                },
                adapters: {
                    date: require("chartjs-adapter-moment"),
                },
                ticks: {
                    autoSkip: true,
                    maxTicksLimit: 8,
                },
            },
            y: {
                //beginAtZero: true,
            },
        },
    };

    return <Line ref={chartRef} data={data} options={options} />;
}
