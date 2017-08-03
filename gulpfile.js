/*
* gulpfile
*/

const gulp = require('gulp');
const install = require('gulp-install');

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
});
