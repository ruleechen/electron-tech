/*
* gulpfile
*/

const gulp = require('gulp');
const install = require('gulp-install');
const builder = require('electron-builder');

gulp.task('build-window-binding', () => (
  gulp.src('./src/components/window-binding/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build-connect-netsdk', () => (
  gulp.src('./src/components/connect-netsdk/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build', ['build-window-binding', 'build-connect-netsdk'], () => {
  builder.build({
    targets: builder.Platform.WINDOWS.createTarget(),
    config: {
      appId: 'com.ringcentral.sfb',
      copyright: 'Copyright © 1999-2017 RingCentral, Inc.',
      compression: 'normal',
      extraResources: [
        'src/components/connect-netsdk/output/**/*',
      ],
      dmg: {},
      linux: {},
      win: {
        // icon: 'build/icon.ico',
        publisherName: 'RingCentral, Inc.',
        target: [{
          target: 'nsis',
          arch: ['ia32'],
        }],
      },
      nsis: {
        include: './build/installer.nsh',
        oneClick: false,
        allowToChangeInstallationDirectory: true,
        createDesktopShortcut: true,
        // shortcutName: '',
        // displayLanguageSelector: true,
        // installerLanguages: ['de-DE', 'en-US'],
        runAfterFinish: true,
        // installerIcon: '',
        // license: 'path to EULA',
        artifactName: '${productName}_${version}_${arch}.${ext}',
      },
      // publish: {
      //   provider: 'generic',
      //   url: 'https://myDownloadUrl',
      // },
      directories: {
        output: './dist/release',
      },
    },
  }).then(() => {
    console.log('done');
  }).catch((error) => {
    console.error(error);
  });
});
