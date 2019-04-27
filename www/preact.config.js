// these props are both optional
const CompressionPlugin = require('compression-webpack-plugin');
//const copyPlugin = require('copy-webpack-plugin');
//const path = require('path');
const webpack = require('webpack');

module.exports = (config, env, helpers) => {
	// build with yarn build -p --no-prerender
	if (env.production) {
		let [plugin] = helpers.getPluginsByName(config, 'UglifyJsPlugin');
		config.plugins.push(
			new webpack.optimize.LimitChunkCountPlugin({
				maxChunks: 1
			})
		);
		if (plugin && plugin.length) {
			plugin.plugin.options.sourceMap = false;
			plugin.plugin.options.comments = false;
		}
		else {
			config.plugins.push(
				new webpack.optimize.UglifyJsPlugin({
					sourceMap: false
				})
			);
		}
		config.plugins.push(
			new CompressionPlugin({
				threshold: 0,
				deleteOriginalAssets: true
			})
		);
	}
};
