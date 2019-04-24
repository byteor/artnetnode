import { h, Component } from 'preact';
import { route } from 'preact-router';
import TopAppBar from 'preact-material-components/TopAppBar';
import Drawer from 'preact-material-components/Drawer';
import List from 'preact-material-components/List';
import 'preact-material-components/Drawer/style.css';
import 'preact-material-components/List/style.css';
import 'preact-material-components/TopAppBar/style.css';

export default class Header extends Component {
	closeDrawer() {
		this.drawer.MDComponent.open = false;
		this.state = {
			darkThemeEnabled: false
		};
	}

	openDrawer = () => (this.drawer.MDComponent.open = true);

	drawerRef = drawer => (this.drawer = drawer);

	linkTo = path => () => {
		route(path);
		this.closeDrawer();
	};

	goHome = this.linkTo('/');
	goToMyProfile = this.linkTo('/profile');
	goToNetwork = this.linkTo('/net');
	goToWiFi = this.linkTo('/wifi');
	goToHW = this.linkTo('/hardware');
	goToDMX = this.linkTo('/dmx');

	render(props) {
		console.log(props.selectedRoute);
		return (
			<div>
				<TopAppBar className="topappbar">
					<TopAppBar.Row>
						<TopAppBar.Section align-start>
							<TopAppBar.Icon menu onClick={this.openDrawer}>
								menu
							</TopAppBar.Icon>
							<TopAppBar.Title>ArtNet Node</TopAppBar.Title>
						</TopAppBar.Section>
					</TopAppBar.Row>
				</TopAppBar>
				<Drawer modal ref={this.drawerRef}>
					<Drawer.DrawerContent>
						<Drawer.DrawerItem selected={props.selectedRoute === '/'} onClick={this.goHome}>
							<List.ItemGraphic>home</List.ItemGraphic>
							Dashboard
						</Drawer.DrawerItem>
						<Drawer.DrawerItem selected={props.selectedRoute === '/net'} onClick={this.goToNetwork}>
							<List.ItemGraphic>settings_ethernet</List.ItemGraphic>
							Network
						</Drawer.DrawerItem>
						<Drawer.DrawerItem selected={props.selectedRoute === '/wifi'} onClick={this.goToWiFi}>
							<List.ItemGraphic>wifi</List.ItemGraphic>
							WiFi
						</Drawer.DrawerItem>
						<Drawer.DrawerItem selected={props.selectedRoute === '/dmx'} onClick={this.goToDMX}>
							<List.ItemGraphic>settings_remote</List.ItemGraphic>
							DMX
						</Drawer.DrawerItem>
						<Drawer.DrawerItem selected={props.selectedRoute === '/hardware'} onClick={this.goToHW}>
							<List.ItemGraphic>developer_board</List.ItemGraphic>
							Hardware
						</Drawer.DrawerItem>
					</Drawer.DrawerContent>
				</Drawer>
			</div>
		);
	}
}
