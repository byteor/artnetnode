// these props are both optional
const CompressionPlugin = require('compression-webpack-plugin');
const UglifyJsPlugin = require('uglifyjs-webpack-plugin');
const webpack = require('webpack');

module.exports = (config, env, helpers) => {
	// build with yarn build --no-prerender

	let [ plugin ] = helpers.getPluginsByName(config, 'UglifyJsPlugin');
	config.plugins.push(
		new webpack.optimize.LimitChunkCountPlugin({
			maxChunks: 1
		})
	);
	if (env.production) {
		if (plugin && plugin.length) {
			plugin.plugin.options.sourceMap = false;
			plugin.plugin.options.comments = false;
		} else {
			config.plugins.push(
				new UglifyJsPlugin({
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
