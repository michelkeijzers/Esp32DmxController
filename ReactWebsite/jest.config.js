module.exports = {
  testEnvironment: 'jsdom',
  transform: {
    '^.+\\.[jt]sx?$': 'babel-jest',
  },
  roots: ['<rootDir>/__tests__'],
  setupFilesAfterEnv: [require.resolve('./jest.setup.js')],
  moduleNameMapper: {
    '\\.(css|less|scss|sass)$': 'identity-obj-proxy',
    '^react-markdown$': require.resolve('./__mocks__/react-markdown.js'),
    '^remark-gfm$': require.resolve('./__mocks__/remark-gfm.js')
  },
  transformIgnorePatterns: [
    '/node_modules/(?!(react-markdown|devlop|remark-gfm|hast-util-to-jsx-runtime|comma-separated-tokens|estree-util-is-identifier-name|hast-util-whitespace|property-information|space-separated-tokens|unist-util-position|vfile-message|unist-util-stringify-position|html-url-attributes|remark-parse|mdast-util-from-markdown|mdast-util-to-string|micromark|decode-named-character-reference)/)'
  ]
};
