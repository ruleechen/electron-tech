/*
* gulpfile
*/

const gulp = require('gulp');
const install = require('gulp-install');

gulp.task('install-components', () => (
  gulp.src('./src/components/window-binding/package.json')
    .pipe(install({ production: true }))
));

gulp.task('build', ['install-components'], () => {
});
