
import { BridgeTS } from './BridgeTS';
import { Window } from './Window';
import { App, log } from'./App';
import { TestJSBinding } from'./TestJSBinding';


declare var window: Window;
window.bridgets = BridgeTS.getInstance();
window.app = new App(BigInt(0));

function tester() {
    let value1 = BridgeTS.add(5, 6); 
    log('add 5 and 6 = ' + value1);

    let object = new TestJSBinding();
    object.setNumber(10);
    let value2 = object.getNumber();
    log('new TestJSBinding getNumber should be 10 and it is: ' + value2);

    let value3 = TestJSBinding.randomNumber(10);
    log('new TestJSBinding randomNumber to 10: ' + value3);
    
}
// @ts-ignore
window.tester = tester;
