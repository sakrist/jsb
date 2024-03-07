function FileSystem() {
  this.syncfs = function (arg1, arg2) {
    if (typeof arg1 === 'function') {
      const callback = arg1;
      // Implement syncfs logic here
      // ...
      callback(null); // Dummy callback for illustration
    } else {
      const doSync = arg1;
      const callback = arg2;

      callback(null); // Dummy callback for illustration
    }
  };

  this.mount = function (type, opts, mountpoint) {
      
  };
    
  this.createPath = function (path, directory) {
      window.Module.FilesystemJS.createPath(path, directory);
  };

  this.readdir = function (path) {
    return JSON.parse(window.Module.FilesystemJS.readdir(path));
  };

  this.rename = function (oldPath, newPath) {
      window.Module.FilesystemJS.rename(oldPath, newPath);
  };

  this.writeFile = function (path, data) {
      let encodedData;

      if (data instanceof Uint8Array) {
          const decoder = new TextDecoder('ascii');
          encodedData = decoder.decode(data, { stream: true }); // Treat data as a stream of bytes
      } else if (typeof data === 'string') {
          encodedData = data;
      } else {
          throw new Error('Unsupported data type. Only Uint8Array or string is supported.');
      }

      // Call your native writeFile function with the encoded data
      window.Module.FilesystemJS.writeFile(path, encodedData);
  };
    
  this.readFile = function (path, encoding) {
      return window.Module.FilesystemJS.readFile(path);
  };

  this.rmdir = function (path) {
      window.Module.FilesystemJS.remove(path);
  };

  this.unlink = function (path) {
      window.Module.FilesystemJS.remove(path);
  };

  this.analyzePath = function (path) {
    return JSON.parse(window.Module.FilesystemJS.analyzePath(path));
  };
}
window.FS = new FileSystem();
