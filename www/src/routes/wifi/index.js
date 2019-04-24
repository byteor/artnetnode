import { h, Component } from 'preact';
import linkState from 'linkstate';
import Button from 'preact-material-components/Button';
import 'preact-material-components/Button/style.css';

import Fab from 'preact-material-components/Fab';
import Icon from 'preact-material-components/Icon';
import 'preact-material-components/Fab/style.css';

import List from 'preact-material-components/List';
import 'preact-material-components/List/style.css';
//import 'preact-material-components/Menu/style.css';

import Dialog from 'preact-material-components/Dialog';
import 'preact-material-components/Dialog/style.css';

import TextField from 'preact-material-components/TextField';
import 'preact-material-components/TextField/style.css';

import Switch from 'preact-material-components/Switch';
import 'preact-material-components/Switch/style.css';

import style from './style';

const WiFiNetwork = ({ network, onRemove, onEdit }) => (
	<List.Item selected={true}>
		<List.ItemGraphic>
			<Icon>wifi</Icon>
		</List.ItemGraphic>
		<List.TextContainer>
			<List.PrimaryText>{network.ssid}</List.PrimaryText>
			<List.SecondaryText>{network.dhcp ? 'DHCP' : network.ip}</List.SecondaryText>
		</List.TextContainer>
		<List.ItemMeta>
			<Button onClick={onEdit}><Icon>edit</Icon></Button>
			<Button onClick={onRemove}><Icon>delete</Icon></Button>
		</List.ItemMeta>
	</List.Item>
);

const WiFiNetworks = ({ networks, onRemove, onEdit }) => (
	<List avatar-list={true} two-line={true} >
		{networks.map(net => (
			<WiFiNetwork key={net.ssid} network={net} onRemove={onRemove(net)} onEdit={onEdit(net)} />
		))}
	</List>
);

class EditNetwork extends Component {
	state = {
		visible: this.props.visible,
		network: { ...this.props.network }
	};

	componentWillReceiveProps = newProps => {
		console.log('newProps', newProps);
		if (newProps.visible !== this.state.visible && this.dlg) {
			if (newProps.visible) this.dlg.MDComponent.show(); else this.dlg.MDComponent.close();
		}
		this.setState(state => ({
			...state,
			visible: newProps.visible,
			network: { ...newProps.network },
		}));
	};

	save = () => {
		const { onSave } = this.props;
		const { network } = this.state;
		onSave(network);
	}

	cancel = () => {
		this.setState(state => ({
			...state,
			visible: false,
		}));
	};

	render = ({ name }, { network, visible }) => {
		console.log('render', name, network, 'visible', visible);
		return (
			<Dialog ref={dlg => this.dlg = dlg}>
				<Dialog.Header>{`${name} WiFi Network`}</Dialog.Header>
				<Dialog.Body>
					<div class={`${style.dialogBody}`}>
						<TextField label="SSID" value={network.ssid} onInput={linkState(this, 'network.ssid')} />
						<TextField label="Password" value={network.pwd} onInput={linkState(this, 'network.pwd')} />
						<div>
							<span>DHCP</span>
							<Switch disabled checked />
						</div>
					</div>
				</Dialog.Body>
				<Dialog.Footer>
					<Dialog.FooterButton cancel={true} onClick={this.save.bind(this)}>Ok</Dialog.FooterButton>
					<Dialog.FooterButton accept={true} onClick={this.cancel.bind(this)}>Cancel</Dialog.FooterButton>
				</Dialog.Footer>
			</Dialog>
		)
	};
}


export default class WiFi extends Component {
	state = {
		wifi: [
			{ _id: '1', ssid: 'ssid1', pwd: 'pwd1', dhcp: true },
			{ _id: '2', ssid: 'ssid2', pwd: 'pwd2', dhcp: false, ip: '111.222.333.4', netmask: '', dns: '' },
		],
		showAddDialog: false,
		showEditDialog: false,
	};

	editWifi = {};

	componentDidMount = () => {
		// TODO: !!! FETCH !!!
	}

	persist = net => {
		// TODO: !!! PERSIST !!!
	}

	onShowWifiEdit = net => () => {
		this.editWifi = net;
		this.setState({ showEditDialog: true });
	}

	onShowWifiAdd = () => {
		this.editWifi = {};
		this.setState({ showAddDialog: true });
	}

	onShowWifiRemove = net => () => {
		this.editWifi = net;
		this.removeDlg.MDComponent.show();
	}

	onWifiRemove = () => {
		if (this.editWifi) {
			const { wifi } = this.state;
			this.setState({ wifi: wifi.filter(net => net !== this.editWifi) });
		}
	}

	onWifiSave = (net) => {
		this.setState(oldState => ({
			...oldState,
			showAddDialog: false,
			showEditDialog: false,
			wifi: oldState.wifi.map(item => item._id === net._id ? net : item)
		}));
		this.persist(net);
	}

	onWifiAdd = (net) => {
		net._id = '_' + Math.random().toString(36).substr(2, 9);
		this.setState(oldState => ({
			...oldState,
			showAddDialog: false,
			showEditDialog: false,
			wifi: oldState.wifi.concat(net)
		}));
		this.persist(net);
	}

	render({ }, { wifi, showAddDialog, showEditDialog }) {
		return (
			<div class={`${style.home} page`}>
				<h3>Stored WiFi Networks</h3>
				<WiFiNetworks networks={wifi} onEdit={this.onShowWifiEdit} onRemove={this.onShowWifiRemove} />
				<div class={`${style.fabAdd}`}>
					<Fab onClick={this.onShowWifiAdd} primary>
						<Icon>add</Icon>
					</Fab>
				</div>
				<Dialog ref={dlg => this.removeDlg = dlg}>
					{/* TODO: WIFI network name */}
					<Dialog.Header>{`Remove WiFi Network?`}</Dialog.Header>
					<Dialog.Body>
						This operation cannot be undone
 					</Dialog.Body>
					<Dialog.Footer>
						<Dialog.FooterButton cancel={true} onClick={this.onWifiRemove}>Ok</Dialog.FooterButton>
						<Dialog.FooterButton accept={true}>Cancel</Dialog.FooterButton>
					</Dialog.Footer>
				</Dialog>
				<EditNetwork name="Edit" network={this.editWifi} onSave={this.onWifiSave.bind(this)} visible={showEditDialog} />
				<EditNetwork name="Add" network={{}} onSave={this.onWifiAdd.bind(this)} visible={showAddDialog} />
			</div>
		);
	}
}
