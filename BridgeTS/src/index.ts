
import { BridgeTS } from './BridgeTS';
import { Window } from './Window';


declare var window: Window;
window.bridgets = BridgeTS.getInstance();