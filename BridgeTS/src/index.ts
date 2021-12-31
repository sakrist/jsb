
import { BridgeTS } from './BridgeTS';
import { Window } from './Window';
import { App } from'./App';



declare var window: Window;
window.bridgets = BridgeTS.getInstance();
window.app = new App(BigInt(0));


