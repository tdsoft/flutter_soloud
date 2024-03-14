part of 'exceptions.dart';

// ////////////////////////
// / Dart-side exceptions /
// ////////////////////////

/// An exception that is thrown when the SoLoud isolate fails to spawn.
class SoLoudIsolateSpawnFailedException extends SoLoudDartException {
  /// Creates a new [SoLoudIsolateSpawnFailedException].
  const SoLoudIsolateSpawnFailedException([super.message]);

  @override
  String get description => 'The audio isolate failed to spawn.';
}

/// An exception that is thrown when a SoLoud method is accessed before
/// the engine is initialized (by calling `SoLoud.initialize()` and either
/// awaiting that method call, or awaiting the `SoLoud.initialized` Future
/// elsewhere, or by checking the synchronous `SoLoud.isInitialized` bool).
class SoLoudNotInitializedException extends SoLoudDartException {
  /// Creates a new [SoLoudNotInitializedException].
  const SoLoudNotInitializedException([super.message]);

  @override
  String get description => 'SoLoud has not been initialized yet. '
      'Call `SoLoud.initialize()` first and await it, or await the '
      '`SoLoud.initialized` Future elsewhere. Alternately, you can check '
      'the synchronous `SoLoud.isInitialized` bool just before calling '
      'any SoLoud method.';
}

/// An exception that is thrown when the SoLoud engine initialization times out.
class SoLoudInitializationTimedOutException extends SoLoudDartException {
  /// Creates a new [SoLoudInitializationTimedOutException].
  const SoLoudInitializationTimedOutException([super.message]);

  @override
  String get description => 'The engine took too long to initialize.';
}

/// An exception that is thrown when the SoLoud engine fails to shutdown.
/// This is not thrown during normal shutdown, but it _can_ be thrown if
/// `initialize()` was called at a time of shutdown, and that shutdown failed.
class ShutdownFailedException extends SoLoudDartException {
  /// Creates a new [ShutdownFailedException].
  const ShutdownFailedException([super.message]);

  @override
  String get description => 'The engine failed to shut down. '
      'This is not thrown during normal shutdown, but it _can_ be thrown if '
      '`initialize()` was called at a time of a shutdown in progress, '
      'and that shutdown failed.';
}

/// An exception that is thrown when the temporary folder fails to be created
/// or opened.
class SoLoudTemporaryFolderFailedException extends SoLoudDartException {
  /// Creates a new [SoLoudTemporaryFolderFailedException].
  const SoLoudTemporaryFolderFailedException([super.message]);

  @override
  String get description => 'There was a problem creating or opening the '
      'temporary folder that is used to hold audio files.';
}

/// An exception that is thrown when the network request returns
/// a non-200 status code.
class SoLoudNetworkStatusCodeException extends SoLoudDartException {
  /// Creates a new [SoLoudNetworkStatusCodeException].
  const SoLoudNetworkStatusCodeException(this.statusCode, [String? message])
      : super(message);

  /// The status code returned from the network request.
  final int statusCode;

  @override
  String get description => 'The request failed with status code $statusCode.';
}

/// An exception that is thrown when SoLoud (Dart) receives a sound hash
/// that is not found.
class SoLoudSoundHashNotFoundDartException extends SoLoudDartException {
  /// Creates a new [SoLoudSoundHashNotFoundDartException].
  const SoLoudSoundHashNotFoundDartException(this.soundHash, [String? message])
      : super(message);

  /// The sound hash that was not found.
  final SoundHash soundHash;

  @override
  String get description => 'The sound with specified hash is not found '
      '(on the Dart side).';
}
