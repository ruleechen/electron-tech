/*
* freeze
*/

module.exports = {
  create: ({ timeout }) => {
    let frozen = false;
    return () => {
      if (frozen) {
        return true;
      }
      frozen = true;
      setTimeout(() => {
        frozen = false;
      }, timeout);
      return false;
    };
  },
};
