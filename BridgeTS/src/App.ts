import { JSBinding } from './JSBinding';
import { TestJSBinding } from './TestJSBinding'

export class App extends JSBinding  {
    public constructor(ptrObject: BigInt) {
        super(ptrObject);
    }

    public static getTestJSBinding(objectPtr:BigInt) {
        let object = new TestJSBinding(objectPtr);
        object.setNumber(2);
        console.log(object);
        object.getNumber().then(function(value:number) { log('Now finally done with: ' + value) });
      }
}

function log(txt:string) {
    // @ts-ignore
    document.getElementById('msg').innerHTML += txt + '<br>'
  }