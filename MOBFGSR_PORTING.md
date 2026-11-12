# Mob-FGSR Android Port — Stage 1

This branch is an isolated research workspace for evaluating a non-root Android
screen-capture and frame-interpolation path on a Poco F5 (Snapdragon 7+ Gen 2,
Adreno 725).

## Stage 1 scope

- Preserve the existing Android application's MediaProjection and overlay
  foundation while keeping all experiments on this branch.
- Establish a repeatable GitHub Actions debug-APK build.
- Set the initial capture target to **1280 × 720 (720p)** for the next
  implementation stage.
- Do not claim Mob-FGSR interpolation is integrated yet.

## Important technical boundary

The upstream Mob-FGSR sample is an offline OpenGL desktop program. Its
interpolation path expects color frames plus depth and motion-vector inputs.
A normal Android game does not expose those buffers to an unrelated,
non-root application. MediaProjection provides captured screen pixels, not
the game's original depth or motion vectors.

The proposed Android research path is therefore:

1. Capture screen frames through Android MediaProjection.
2. Estimate screen-space motion from consecutive color frames (optical flow).
3. Adapt the Mob-FGSR warping/blending concepts to the estimated motion.
4. Present the generated image through an overlay and measure end-to-end
   latency, visual artifacts, and GPU cost.

This is an adaptation/research path, not a drop-in port of the original
depth-and-motion-vector pipeline. It may not work reliably in every game.

## Device target

- Device: Poco F5
- SoC/GPU: Snapdragon 7+ Gen 2 / Adreno 725
- Android: Android 15
- Root: not required
- Initial capture target: 1280 × 720

## Stage 1 validation

Run the **Mob-FGSR Android Stage 1** workflow manually from the Actions tab,
or push a commit to this branch. It attempts to compile the existing Android
debug variant and uploads any generated APK as a workflow artifact.

A successful build validates the build pipeline only. It does not yet validate
720p capture, optical flow, frame interpolation, or overlay synchronization.

## Next stages

- Stage 2: verify the current MediaProjection capture path and add a measurable
  1280 × 720 capture configuration.
- Stage 3: implement and benchmark a lightweight optical-flow estimator.
- Stage 4: adapt the required Mob-FGSR compute stages for Android GPU APIs.
- Stage 5: integrate generated frames into the overlay and measure latency.
