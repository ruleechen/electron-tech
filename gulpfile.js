/*
* gulpfile
*/

import gulp from 'gulp';
import install from 'gulp-install';
import rebuild from 'electron-rebuild';

const builder = require('electron-builder');

gulp.task('build-window-binding', () => (
  // gulp.src('./src/components/window-binding/package.json')
  // .pipe(install({ production: true }))
  rebuild({
    buildPath: `${__dirname}/src/components/window-binding`,
    electronVersion: '1.7.9',
    arch: 'x64',
  })
));

gulp.task('build-connect-netsdk', () => (
  gulp.src('./src/components/connect-netsdk/package.json')
    .pipe(install({ production: true }))
));

gulp.task('prebuild', ['build-window-binding', 'build-connect-netsdk']);

gulp.task('build', ['prebuild'], () => {
  builder.build({
    targets: builder.Platform.WINDOWS.createTarget(),
    config: {
      appId: 'com.ringcentral.rc4sfb',
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
          arch: ['ia32'], // ia32,x64
        }],
      },
      nsis: {
        guid: '69ace1b2-ace4-493b-9eb2-251a49be689d',
        include: './build/installer.nsh',
        oneClick: false,
        allowToChangeInstallationDirectory: true,
        createDesktopShortcut: true,
        allowElevation: true,
        runAfterFinish: true,
        // shortcutName: '',
        // displayLanguageSelector: true,
        // installerLanguages: ['de-DE', 'en-US'],
        // installerIcon: '',
        license: './build/eula.html',
        artifactName: '${productName}_${version}_${arch}.${ext}',
        // publish: {
        //   provider: 'generic',
        //   url: 'https://myDownloadUrl',
        // },
      },
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
