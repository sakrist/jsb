
import { BridgeTS } from './BridgeTS';
import { Window } from './Window';
import { JSBinding } from './JSBinding';
import { TestJSBinding } from'./TestJSBinding';


declare var window: Window;
window.bridgets = BridgeTS.getInstance();

export function log(txt:string) {
    // @ts-ignore
    document.getElementById('msg').innerHTML += txt + '<br>'
}


function tester() {
    let object = new TestJSBinding();
    object.setNumber(10);
    let value2 = object.getNumber();
    log('new TestJSBinding getNumber should be 10 and it is: ' + value2);

    let value3 = TestJSBinding.randomNumber(10);
    log('new TestJSBinding randomNumber to 10: ' + value3);
    
}
// @ts-ignore
window.tester = tester;
// var t = new TestJSBinding(BigInt(1));
// console.log(t);