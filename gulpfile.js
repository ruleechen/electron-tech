/*
* gulpfile
*/

const gulp = require('gulp');
const install = require('gulp-install');
const builder = require('electron-builder');

const isWindows = /^win/.test(process.platform);

gulp.task('build-window-binding', () => (
  gulp.src('./src/components/window-binding/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build-connect-netsdk', () => (
  gulp.src('./src/components/connect-netsdk/package.json')
    .pipe(install({ production: true }))
));

const components = ['build-window-binding'];
if (isWindows) {
  components.push('build-connect-netsdk');
}

gulp.task('build', components, () => {
  builder.build({
    config: {
      appId: 'com.ringcentral.sfb',
      dmg: {
        contents: [
          {
            x: 110,
            y: 150,
          },
          {
            x: 240,
            y: 150,
            type: 'link',
            path: '/Applications',
          },
        ],
      },
      linux: {
        target: [
          'AppImage',
          'deb',
        ],
      },
      win: {
        target: 'Squirrel.Windows',
      },
    },
  }).then(() => {
    console.log('done');
  }).catch((error) => {
    console.error(error);
  });
});
