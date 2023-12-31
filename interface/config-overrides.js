const { WebpackManifestPlugin } = require("webpack-manifest-plugin");
const WorkboxWebpackPlugin = require("workbox-webpack-plugin");
const MiniCssExtractPlugin = require("mini-css-extract-plugin");
const CompressionPlugin = require("compression-webpack-plugin");

const path = require("path");
const fs = require("fs");

module.exports = function override(config, env) {
    if (env === "production") {
        // rename the ouput file, we need it's path to be short, for SPIFFS
        config.output.filename = "js/[id].[chunkhash:4].js";
        config.output.chunkFilename = "js/[id].[chunkhash:4].js";

        // take out the manifest and service worker plugins
        config.plugins = config.plugins.filter(
            (plugin) => !(plugin instanceof WebpackManifestPlugin)
        );
        config.plugins = config.plugins.filter(
            (plugin) => !(plugin instanceof WorkboxWebpackPlugin.GenerateSW)
        );

        // shorten css filenames
        const miniCssExtractPlugin = config.plugins.find(
            (plugin) => plugin instanceof MiniCssExtractPlugin
        );
        miniCssExtractPlugin.options.filename = "css/[id].[contenthash:4].css";
        miniCssExtractPlugin.options.chunkFilename =
            "css/[id].[contenthash:4].c.css";

        // add compression plugin, compress javascript
        config.plugins.push(
            new CompressionPlugin({
                filename: "[path][base]",
                algorithm: "gzip",
                test: /\.(js)$/,
                deleteOriginalAssets: true,
            })
        );
    }

    return config;
};
