// source: system.proto
/**
 * @fileoverview
 * @enhanceable
 * @suppress {missingRequire} reports error on implicit type usages.
 * @suppress {messageConventions} JS Compiler reports an error if a variable or
 *     field starts with 'MSG_' and isn't a translatable message.
 * @public
 */
// GENERATED CODE -- DO NOT EDIT!
/* eslint-disable */
// @ts-nocheck

var jspb = require('google-protobuf');
var goog = jspb;
var global =
    (typeof globalThis !== 'undefined' && globalThis) ||
    (typeof window !== 'undefined' && window) ||
    (typeof global !== 'undefined' && global) ||
    (typeof self !== 'undefined' && self) ||
    (function () { return this; }).call(null) ||
    Function('return this')();

var ludo_pb = require('./ludo_pb.js');
goog.object.extend(proto, ludo_pb);
var google_protobuf_empty_pb = require('google-protobuf/google/protobuf/empty_pb.js');
goog.object.extend(proto, google_protobuf_empty_pb);
goog.exportSymbol('proto.alphaludo.FileName', null, global);
goog.exportSymbol('proto.alphaludo.FileNames', null, global);
goog.exportSymbol('proto.alphaludo.MoveRequest', null, global);
goog.exportSymbol('proto.alphaludo.NewGameConfig', null, global);
goog.exportSymbol('proto.alphaludo.PlayerConfig', null, global);
goog.exportSymbol('proto.alphaludo.PlayerMode', null, global);
goog.exportSymbol('proto.alphaludo.StateResponse', null, global);
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.alphaludo.FileNames = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, proto.alphaludo.FileNames.repeatedFields_, null);
};
goog.inherits(proto.alphaludo.FileNames, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.alphaludo.FileNames.displayName = 'proto.alphaludo.FileNames';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.alphaludo.FileName = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, null);
};
goog.inherits(proto.alphaludo.FileName, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.alphaludo.FileName.displayName = 'proto.alphaludo.FileName';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.alphaludo.PlayerConfig = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, proto.alphaludo.PlayerConfig.repeatedFields_, null);
};
goog.inherits(proto.alphaludo.PlayerConfig, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.alphaludo.PlayerConfig.displayName = 'proto.alphaludo.PlayerConfig';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.alphaludo.NewGameConfig = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, proto.alphaludo.NewGameConfig.repeatedFields_, null);
};
goog.inherits(proto.alphaludo.NewGameConfig, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.alphaludo.NewGameConfig.displayName = 'proto.alphaludo.NewGameConfig';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.alphaludo.MoveRequest = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, null, null);
};
goog.inherits(proto.alphaludo.MoveRequest, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.alphaludo.MoveRequest.displayName = 'proto.alphaludo.MoveRequest';
}
/**
 * Generated by JsPbCodeGenerator.
 * @param {Array=} opt_data Optional initial data array, typically from a
 * server response, or constructed directly in Javascript. The array is used
 * in place and becomes part of the constructed object. It is not cloned.
 * If no data is provided, the constructed object will be empty, but still
 * valid.
 * @extends {jspb.Message}
 * @constructor
 */
proto.alphaludo.StateResponse = function(opt_data) {
  jspb.Message.initialize(this, opt_data, 0, -1, proto.alphaludo.StateResponse.repeatedFields_, null);
};
goog.inherits(proto.alphaludo.StateResponse, jspb.Message);
if (goog.DEBUG && !COMPILED) {
  /**
   * @public
   * @override
   */
  proto.alphaludo.StateResponse.displayName = 'proto.alphaludo.StateResponse';
}

/**
 * List of repeated fields within this message type.
 * @private {!Array<number>}
 * @const
 */
proto.alphaludo.FileNames.repeatedFields_ = [1];



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.alphaludo.FileNames.prototype.toObject = function(opt_includeInstance) {
  return proto.alphaludo.FileNames.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.alphaludo.FileNames} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.FileNames.toObject = function(includeInstance, msg) {
  var f, obj = {
    filesList: (f = jspb.Message.getRepeatedField(msg, 1)) == null ? undefined : f
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.alphaludo.FileNames}
 */
proto.alphaludo.FileNames.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.alphaludo.FileNames;
  return proto.alphaludo.FileNames.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.alphaludo.FileNames} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.alphaludo.FileNames}
 */
proto.alphaludo.FileNames.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {string} */ (reader.readString());
      msg.addFiles(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.alphaludo.FileNames.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.alphaludo.FileNames.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.alphaludo.FileNames} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.FileNames.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getFilesList();
  if (f.length > 0) {
    writer.writeRepeatedString(
      1,
      f
    );
  }
};


/**
 * repeated string files = 1;
 * @return {!Array<string>}
 */
proto.alphaludo.FileNames.prototype.getFilesList = function() {
  return /** @type {!Array<string>} */ (jspb.Message.getRepeatedField(this, 1));
};


/**
 * @param {!Array<string>} value
 * @return {!proto.alphaludo.FileNames} returns this
 */
proto.alphaludo.FileNames.prototype.setFilesList = function(value) {
  return jspb.Message.setField(this, 1, value || []);
};


/**
 * @param {string} value
 * @param {number=} opt_index
 * @return {!proto.alphaludo.FileNames} returns this
 */
proto.alphaludo.FileNames.prototype.addFiles = function(value, opt_index) {
  return jspb.Message.addToRepeatedField(this, 1, value, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.alphaludo.FileNames} returns this
 */
proto.alphaludo.FileNames.prototype.clearFilesList = function() {
  return this.setFilesList([]);
};





if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.alphaludo.FileName.prototype.toObject = function(opt_includeInstance) {
  return proto.alphaludo.FileName.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.alphaludo.FileName} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.FileName.toObject = function(includeInstance, msg) {
  var f, obj = {
    file: jspb.Message.getFieldWithDefault(msg, 1, "")
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.alphaludo.FileName}
 */
proto.alphaludo.FileName.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.alphaludo.FileName;
  return proto.alphaludo.FileName.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.alphaludo.FileName} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.alphaludo.FileName}
 */
proto.alphaludo.FileName.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {string} */ (reader.readString());
      msg.setFile(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.alphaludo.FileName.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.alphaludo.FileName.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.alphaludo.FileName} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.FileName.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getFile();
  if (f.length > 0) {
    writer.writeString(
      1,
      f
    );
  }
};


/**
 * optional string file = 1;
 * @return {string}
 */
proto.alphaludo.FileName.prototype.getFile = function() {
  return /** @type {string} */ (jspb.Message.getFieldWithDefault(this, 1, ""));
};


/**
 * @param {string} value
 * @return {!proto.alphaludo.FileName} returns this
 */
proto.alphaludo.FileName.prototype.setFile = function(value) {
  return jspb.Message.setProto3StringField(this, 1, value);
};



/**
 * List of repeated fields within this message type.
 * @private {!Array<number>}
 * @const
 */
proto.alphaludo.PlayerConfig.repeatedFields_ = [2];



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.alphaludo.PlayerConfig.prototype.toObject = function(opt_includeInstance) {
  return proto.alphaludo.PlayerConfig.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.alphaludo.PlayerConfig} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.PlayerConfig.toObject = function(includeInstance, msg) {
  var f, obj = {
    mode: jspb.Message.getFieldWithDefault(msg, 1, 0),
    coloursList: (f = jspb.Message.getRepeatedField(msg, 2)) == null ? undefined : f
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.alphaludo.PlayerConfig}
 */
proto.alphaludo.PlayerConfig.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.alphaludo.PlayerConfig;
  return proto.alphaludo.PlayerConfig.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.alphaludo.PlayerConfig} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.alphaludo.PlayerConfig}
 */
proto.alphaludo.PlayerConfig.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = /** @type {!proto.alphaludo.PlayerMode} */ (reader.readEnum());
      msg.setMode(value);
      break;
    case 2:
      var value = /** @type {string} */ (reader.readString());
      msg.addColours(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.alphaludo.PlayerConfig.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.alphaludo.PlayerConfig.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.alphaludo.PlayerConfig} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.PlayerConfig.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getMode();
  if (f !== 0.0) {
    writer.writeEnum(
      1,
      f
    );
  }
  f = message.getColoursList();
  if (f.length > 0) {
    writer.writeRepeatedString(
      2,
      f
    );
  }
};


/**
 * optional PlayerMode mode = 1;
 * @return {!proto.alphaludo.PlayerMode}
 */
proto.alphaludo.PlayerConfig.prototype.getMode = function() {
  return /** @type {!proto.alphaludo.PlayerMode} */ (jspb.Message.getFieldWithDefault(this, 1, 0));
};


/**
 * @param {!proto.alphaludo.PlayerMode} value
 * @return {!proto.alphaludo.PlayerConfig} returns this
 */
proto.alphaludo.PlayerConfig.prototype.setMode = function(value) {
  return jspb.Message.setProto3EnumField(this, 1, value);
};


/**
 * repeated string colours = 2;
 * @return {!Array<string>}
 */
proto.alphaludo.PlayerConfig.prototype.getColoursList = function() {
  return /** @type {!Array<string>} */ (jspb.Message.getRepeatedField(this, 2));
};


/**
 * @param {!Array<string>} value
 * @return {!proto.alphaludo.PlayerConfig} returns this
 */
proto.alphaludo.PlayerConfig.prototype.setColoursList = function(value) {
  return jspb.Message.setField(this, 2, value || []);
};


/**
 * @param {string} value
 * @param {number=} opt_index
 * @return {!proto.alphaludo.PlayerConfig} returns this
 */
proto.alphaludo.PlayerConfig.prototype.addColours = function(value, opt_index) {
  return jspb.Message.addToRepeatedField(this, 2, value, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.alphaludo.PlayerConfig} returns this
 */
proto.alphaludo.PlayerConfig.prototype.clearColoursList = function() {
  return this.setColoursList([]);
};



/**
 * List of repeated fields within this message type.
 * @private {!Array<number>}
 * @const
 */
proto.alphaludo.NewGameConfig.repeatedFields_ = [1];



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.alphaludo.NewGameConfig.prototype.toObject = function(opt_includeInstance) {
  return proto.alphaludo.NewGameConfig.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.alphaludo.NewGameConfig} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.NewGameConfig.toObject = function(includeInstance, msg) {
  var f, obj = {
    playerConfigsList: jspb.Message.toObjectList(msg.getPlayerConfigsList(),
    proto.alphaludo.PlayerConfig.toObject, includeInstance)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.alphaludo.NewGameConfig}
 */
proto.alphaludo.NewGameConfig.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.alphaludo.NewGameConfig;
  return proto.alphaludo.NewGameConfig.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.alphaludo.NewGameConfig} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.alphaludo.NewGameConfig}
 */
proto.alphaludo.NewGameConfig.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = new proto.alphaludo.PlayerConfig;
      reader.readMessage(value,proto.alphaludo.PlayerConfig.deserializeBinaryFromReader);
      msg.addPlayerConfigs(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.alphaludo.NewGameConfig.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.alphaludo.NewGameConfig.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.alphaludo.NewGameConfig} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.NewGameConfig.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getPlayerConfigsList();
  if (f.length > 0) {
    writer.writeRepeatedMessage(
      1,
      f,
      proto.alphaludo.PlayerConfig.serializeBinaryToWriter
    );
  }
};


/**
 * repeated PlayerConfig player_configs = 1;
 * @return {!Array<!proto.alphaludo.PlayerConfig>}
 */
proto.alphaludo.NewGameConfig.prototype.getPlayerConfigsList = function() {
  return /** @type{!Array<!proto.alphaludo.PlayerConfig>} */ (
    jspb.Message.getRepeatedWrapperField(this, proto.alphaludo.PlayerConfig, 1));
};


/**
 * @param {!Array<!proto.alphaludo.PlayerConfig>} value
 * @return {!proto.alphaludo.NewGameConfig} returns this
*/
proto.alphaludo.NewGameConfig.prototype.setPlayerConfigsList = function(value) {
  return jspb.Message.setRepeatedWrapperField(this, 1, value);
};


/**
 * @param {!proto.alphaludo.PlayerConfig=} opt_value
 * @param {number=} opt_index
 * @return {!proto.alphaludo.PlayerConfig}
 */
proto.alphaludo.NewGameConfig.prototype.addPlayerConfigs = function(opt_value, opt_index) {
  return jspb.Message.addToRepeatedWrapperField(this, 1, opt_value, proto.alphaludo.PlayerConfig, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.alphaludo.NewGameConfig} returns this
 */
proto.alphaludo.NewGameConfig.prototype.clearPlayerConfigsList = function() {
  return this.setPlayerConfigsList([]);
};





if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.alphaludo.MoveRequest.prototype.toObject = function(opt_includeInstance) {
  return proto.alphaludo.MoveRequest.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.alphaludo.MoveRequest} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.MoveRequest.toObject = function(includeInstance, msg) {
  var f, obj = {
    move: (f = msg.getMove()) && ludo_pb.MoveProto.toObject(includeInstance, f),
    moveId: jspb.Message.getFieldWithDefault(msg, 2, 0)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.alphaludo.MoveRequest}
 */
proto.alphaludo.MoveRequest.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.alphaludo.MoveRequest;
  return proto.alphaludo.MoveRequest.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.alphaludo.MoveRequest} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.alphaludo.MoveRequest}
 */
proto.alphaludo.MoveRequest.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = new ludo_pb.MoveProto;
      reader.readMessage(value,ludo_pb.MoveProto.deserializeBinaryFromReader);
      msg.setMove(value);
      break;
    case 2:
      var value = /** @type {number} */ (reader.readInt32());
      msg.setMoveId(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.alphaludo.MoveRequest.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.alphaludo.MoveRequest.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.alphaludo.MoveRequest} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.MoveRequest.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getMove();
  if (f != null) {
    writer.writeMessage(
      1,
      f,
      ludo_pb.MoveProto.serializeBinaryToWriter
    );
  }
  f = message.getMoveId();
  if (f !== 0) {
    writer.writeInt32(
      2,
      f
    );
  }
};


/**
 * optional ludo.MoveProto move = 1;
 * @return {?proto.ludo.MoveProto}
 */
proto.alphaludo.MoveRequest.prototype.getMove = function() {
  return /** @type{?proto.ludo.MoveProto} */ (
    jspb.Message.getWrapperField(this, ludo_pb.MoveProto, 1));
};


/**
 * @param {?proto.ludo.MoveProto|undefined} value
 * @return {!proto.alphaludo.MoveRequest} returns this
*/
proto.alphaludo.MoveRequest.prototype.setMove = function(value) {
  return jspb.Message.setWrapperField(this, 1, value);
};


/**
 * Clears the message field making it undefined.
 * @return {!proto.alphaludo.MoveRequest} returns this
 */
proto.alphaludo.MoveRequest.prototype.clearMove = function() {
  return this.setMove(undefined);
};


/**
 * Returns whether this field is set.
 * @return {boolean}
 */
proto.alphaludo.MoveRequest.prototype.hasMove = function() {
  return jspb.Message.getField(this, 1) != null;
};


/**
 * optional int32 move_id = 2;
 * @return {number}
 */
proto.alphaludo.MoveRequest.prototype.getMoveId = function() {
  return /** @type {number} */ (jspb.Message.getFieldWithDefault(this, 2, 0));
};


/**
 * @param {number} value
 * @return {!proto.alphaludo.MoveRequest} returns this
 */
proto.alphaludo.MoveRequest.prototype.setMoveId = function(value) {
  return jspb.Message.setProto3IntField(this, 2, value);
};



/**
 * List of repeated fields within this message type.
 * @private {!Array<number>}
 * @const
 */
proto.alphaludo.StateResponse.repeatedFields_ = [2,4];



if (jspb.Message.GENERATE_TO_OBJECT) {
/**
 * Creates an object representation of this proto.
 * Field names that are reserved in JavaScript and will be renamed to pb_name.
 * Optional fields that are not set will be set to undefined.
 * To access a reserved field use, foo.pb_<name>, eg, foo.pb_default.
 * For the list of reserved names please see:
 *     net/proto2/compiler/js/internal/generator.cc#kKeyword.
 * @param {boolean=} opt_includeInstance Deprecated. whether to include the
 *     JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @return {!Object}
 */
proto.alphaludo.StateResponse.prototype.toObject = function(opt_includeInstance) {
  return proto.alphaludo.StateResponse.toObject(opt_includeInstance, this);
};


/**
 * Static version of the {@see toObject} method.
 * @param {boolean|undefined} includeInstance Deprecated. Whether to include
 *     the JSPB instance for transitional soy proto support:
 *     http://goto/soy-param-migration
 * @param {!proto.alphaludo.StateResponse} msg The msg instance to transform.
 * @return {!Object}
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.StateResponse.toObject = function(includeInstance, msg) {
  var f, obj = {
    config: (f = msg.getConfig()) && ludo_pb.ConfigProto.toObject(includeInstance, f),
    modesList: (f = jspb.Message.getRepeatedField(msg, 2)) == null ? undefined : f,
    state: (f = msg.getState()) && ludo_pb.StateProto.toObject(includeInstance, f),
    movesList: jspb.Message.toObjectList(msg.getMovesList(),
    ludo_pb.MoveProto.toObject, includeInstance)
  };

  if (includeInstance) {
    obj.$jspbMessageInstance = msg;
  }
  return obj;
};
}


/**
 * Deserializes binary data (in protobuf wire format).
 * @param {jspb.ByteSource} bytes The bytes to deserialize.
 * @return {!proto.alphaludo.StateResponse}
 */
proto.alphaludo.StateResponse.deserializeBinary = function(bytes) {
  var reader = new jspb.BinaryReader(bytes);
  var msg = new proto.alphaludo.StateResponse;
  return proto.alphaludo.StateResponse.deserializeBinaryFromReader(msg, reader);
};


/**
 * Deserializes binary data (in protobuf wire format) from the
 * given reader into the given message object.
 * @param {!proto.alphaludo.StateResponse} msg The message object to deserialize into.
 * @param {!jspb.BinaryReader} reader The BinaryReader to use.
 * @return {!proto.alphaludo.StateResponse}
 */
proto.alphaludo.StateResponse.deserializeBinaryFromReader = function(msg, reader) {
  while (reader.nextField()) {
    if (reader.isEndGroup()) {
      break;
    }
    var field = reader.getFieldNumber();
    switch (field) {
    case 1:
      var value = new ludo_pb.ConfigProto;
      reader.readMessage(value,ludo_pb.ConfigProto.deserializeBinaryFromReader);
      msg.setConfig(value);
      break;
    case 2:
      var values = /** @type {!Array<!proto.alphaludo.PlayerMode>} */ (reader.isDelimited() ? reader.readPackedEnum() : [reader.readEnum()]);
      for (var i = 0; i < values.length; i++) {
        msg.addModes(values[i]);
      }
      break;
    case 3:
      var value = new ludo_pb.StateProto;
      reader.readMessage(value,ludo_pb.StateProto.deserializeBinaryFromReader);
      msg.setState(value);
      break;
    case 4:
      var value = new ludo_pb.MoveProto;
      reader.readMessage(value,ludo_pb.MoveProto.deserializeBinaryFromReader);
      msg.addMoves(value);
      break;
    default:
      reader.skipField();
      break;
    }
  }
  return msg;
};


/**
 * Serializes the message to binary data (in protobuf wire format).
 * @return {!Uint8Array}
 */
proto.alphaludo.StateResponse.prototype.serializeBinary = function() {
  var writer = new jspb.BinaryWriter();
  proto.alphaludo.StateResponse.serializeBinaryToWriter(this, writer);
  return writer.getResultBuffer();
};


/**
 * Serializes the given message to binary data (in protobuf wire
 * format), writing to the given BinaryWriter.
 * @param {!proto.alphaludo.StateResponse} message
 * @param {!jspb.BinaryWriter} writer
 * @suppress {unusedLocalVariables} f is only used for nested messages
 */
proto.alphaludo.StateResponse.serializeBinaryToWriter = function(message, writer) {
  var f = undefined;
  f = message.getConfig();
  if (f != null) {
    writer.writeMessage(
      1,
      f,
      ludo_pb.ConfigProto.serializeBinaryToWriter
    );
  }
  f = message.getModesList();
  if (f.length > 0) {
    writer.writePackedEnum(
      2,
      f
    );
  }
  f = message.getState();
  if (f != null) {
    writer.writeMessage(
      3,
      f,
      ludo_pb.StateProto.serializeBinaryToWriter
    );
  }
  f = message.getMovesList();
  if (f.length > 0) {
    writer.writeRepeatedMessage(
      4,
      f,
      ludo_pb.MoveProto.serializeBinaryToWriter
    );
  }
};


/**
 * optional ludo.ConfigProto config = 1;
 * @return {?proto.ludo.ConfigProto}
 */
proto.alphaludo.StateResponse.prototype.getConfig = function() {
  return /** @type{?proto.ludo.ConfigProto} */ (
    jspb.Message.getWrapperField(this, ludo_pb.ConfigProto, 1));
};


/**
 * @param {?proto.ludo.ConfigProto|undefined} value
 * @return {!proto.alphaludo.StateResponse} returns this
*/
proto.alphaludo.StateResponse.prototype.setConfig = function(value) {
  return jspb.Message.setWrapperField(this, 1, value);
};


/**
 * Clears the message field making it undefined.
 * @return {!proto.alphaludo.StateResponse} returns this
 */
proto.alphaludo.StateResponse.prototype.clearConfig = function() {
  return this.setConfig(undefined);
};


/**
 * Returns whether this field is set.
 * @return {boolean}
 */
proto.alphaludo.StateResponse.prototype.hasConfig = function() {
  return jspb.Message.getField(this, 1) != null;
};


/**
 * repeated PlayerMode modes = 2;
 * @return {!Array<!proto.alphaludo.PlayerMode>}
 */
proto.alphaludo.StateResponse.prototype.getModesList = function() {
  return /** @type {!Array<!proto.alphaludo.PlayerMode>} */ (jspb.Message.getRepeatedField(this, 2));
};


/**
 * @param {!Array<!proto.alphaludo.PlayerMode>} value
 * @return {!proto.alphaludo.StateResponse} returns this
 */
proto.alphaludo.StateResponse.prototype.setModesList = function(value) {
  return jspb.Message.setField(this, 2, value || []);
};


/**
 * @param {!proto.alphaludo.PlayerMode} value
 * @param {number=} opt_index
 * @return {!proto.alphaludo.StateResponse} returns this
 */
proto.alphaludo.StateResponse.prototype.addModes = function(value, opt_index) {
  return jspb.Message.addToRepeatedField(this, 2, value, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.alphaludo.StateResponse} returns this
 */
proto.alphaludo.StateResponse.prototype.clearModesList = function() {
  return this.setModesList([]);
};


/**
 * optional ludo.StateProto state = 3;
 * @return {?proto.ludo.StateProto}
 */
proto.alphaludo.StateResponse.prototype.getState = function() {
  return /** @type{?proto.ludo.StateProto} */ (
    jspb.Message.getWrapperField(this, ludo_pb.StateProto, 3));
};


/**
 * @param {?proto.ludo.StateProto|undefined} value
 * @return {!proto.alphaludo.StateResponse} returns this
*/
proto.alphaludo.StateResponse.prototype.setState = function(value) {
  return jspb.Message.setWrapperField(this, 3, value);
};


/**
 * Clears the message field making it undefined.
 * @return {!proto.alphaludo.StateResponse} returns this
 */
proto.alphaludo.StateResponse.prototype.clearState = function() {
  return this.setState(undefined);
};


/**
 * Returns whether this field is set.
 * @return {boolean}
 */
proto.alphaludo.StateResponse.prototype.hasState = function() {
  return jspb.Message.getField(this, 3) != null;
};


/**
 * repeated ludo.MoveProto moves = 4;
 * @return {!Array<!proto.ludo.MoveProto>}
 */
proto.alphaludo.StateResponse.prototype.getMovesList = function() {
  return /** @type{!Array<!proto.ludo.MoveProto>} */ (
    jspb.Message.getRepeatedWrapperField(this, ludo_pb.MoveProto, 4));
};


/**
 * @param {!Array<!proto.ludo.MoveProto>} value
 * @return {!proto.alphaludo.StateResponse} returns this
*/
proto.alphaludo.StateResponse.prototype.setMovesList = function(value) {
  return jspb.Message.setRepeatedWrapperField(this, 4, value);
};


/**
 * @param {!proto.ludo.MoveProto=} opt_value
 * @param {number=} opt_index
 * @return {!proto.ludo.MoveProto}
 */
proto.alphaludo.StateResponse.prototype.addMoves = function(opt_value, opt_index) {
  return jspb.Message.addToRepeatedWrapperField(this, 4, opt_value, proto.ludo.MoveProto, opt_index);
};


/**
 * Clears the list making it empty but non-null.
 * @return {!proto.alphaludo.StateResponse} returns this
 */
proto.alphaludo.StateResponse.prototype.clearMovesList = function() {
  return this.setMovesList([]);
};


/**
 * @enum {number}
 */
proto.alphaludo.PlayerMode = {
  AI: 0,
  HUMAN: 1
};

goog.object.extend(exports, proto.alphaludo);